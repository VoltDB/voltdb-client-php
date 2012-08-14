extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

#include <map>
#include <vector>
#include <string>
#include "ConnectionPool.h"
#include "Client.h"
#include "Exception.hpp"
#include "Procedure.hpp"
#include "Parameter.hpp"
#include "ParameterSet.hpp"
#include "WireType.h"
#include "common.h"
#include "response.h"
#include "client.h"

// class entry used to instantiate the PHP client class
zend_class_entry *voltclient_ce;

extern zend_class_entry *voltresponse_ce;

const zend_function_entry voltclient_methods[] = {
    // VoltClient
    PHP_ME(VoltClient, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(VoltClient, connect, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, addStoredProc, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, removeStoredProc, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, getStoredProcs, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invoke, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invokeAsync, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, drain, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in zend_function_entry */
};

/* VoltClient */
zend_object_handlers voltclient_object_handlers;

void voltclient_free(void *obj TSRMLS_CC)
{
    voltclient_object *client_obj = (voltclient_object *)obj;

    std::map<const char *, voltdb::Procedure *>::iterator it;
    for (it = client_obj->procedures.begin();
         it != client_obj->procedures.end();
         it++) {
        delete it->second;
    }
    client_obj->procedures.clear();

    // Return the client held by this thread to the pool
    voltdb::ConnectionPool::pool()->onScriptEnd();

    zend_hash_destroy(client_obj->std.properties);
    FREE_HASHTABLE(client_obj->std.properties);

    efree(client_obj);
}

zend_object_value voltclient_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    voltclient_object *obj = (voltclient_object *)emalloc(sizeof(voltclient_object));
    memset(obj, 0, sizeof(voltclient_object));
    obj->std.ce = type;
    obj->procedures = std::map<const char *, voltdb::Procedure *>();

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
                                           voltclient_free, NULL TSRMLS_CC);
    retval.handlers = &voltclient_object_handlers;

    return retval;
}

void create_voltclient_class(void)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltClient", voltclient_methods);
    voltclient_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltclient_ce->create_object = voltclient_create_handler;
    memcpy(&voltclient_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    voltclient_object_handlers.clone_obj = NULL;
}

voltdb::Procedure *prepare_to_invoke(INTERNAL_FUNCTION_PARAMETERS, voltclient_object *obj)
{
    char *name = NULL;
    zval ***params = NULL;
    int argc = ZEND_NUM_ARGS();
    int param_offset = 1;
    int param_count = argc - param_offset;
    int i, len = 0;

    assert(obj != NULL);
    // TODO: Check if the client is created.

    // Get the procedure name
    if (zend_parse_parameters(1 TSRMLS_CC, (char *)"s", &name, &len) == FAILURE ||
        get_varargs(argc, &params) == 0) {
        if (params != NULL) {
            efree(params);
        }
        return NULL;
    }

    // Get the procedure object
    std::map<const char *, voltdb::Procedure *>::const_iterator it = obj->procedures.find(name);
    if (it == obj->procedures.end()) {
        // Procedure doesn't exist
        efree(params);
        return NULL;
    }

    // Set the parameters
    voltdb::errType err = voltdb::errOk;
    voltdb::Procedure *proc = it->second;
    voltdb::ParameterSet *proc_params = proc->params();
    proc_params->reset(err);
    if (!voltdb::isOk(err)) {
        efree(params);
        return NULL;
    }

    // TODO: Only string params for now
    for (i = param_offset; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) == IS_STRING) {
            proc_params->addString(err, std::string(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i])));
        } else {
            zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                 voltdb::errParamMismatchException TSRMLS_CC);
            efree(params);
            return NULL;
        }

        if (!voltdb::isOk(err)) {
            zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
            efree(params);
            return NULL;
        }
    }

    efree(params);

    return proc;
}

PHP_METHOD(VoltClient, __construct)
{
    // No-op, client is created in connect from the ConnectionPool
}

PHP_METHOD(VoltClient, connect)
{
    int argc = ZEND_NUM_ARGS();
    int len = 0;
    char *hostname = NULL;
    char *username = NULL;
    char *password = NULL;
    int port = 21212;

    voltdb::errType err = voltdb::errOk;
    zval *zobj = getThis();
    voltclient_object *obj = (voltclient_object *)zend_object_store_get_object(zobj TSRMLS_CC);

    if (zend_parse_parameters(argc TSRMLS_CC, (char *)"sss|l", &hostname, &len, &username, &len,
                              &password, &len, &port) == FAILURE) {
        RETURN_FALSE;
    }

    if (argc == 4) {
        obj->client = voltdb::ConnectionPool::pool()->acquireClient(err, hostname, username, password, port);
    } else {
        obj->client = voltdb::ConnectionPool::pool()->acquireClient(err, hostname, username, password);
    }

    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(VoltClient, addStoredProc)
{
    char *name = NULL;
    zval ***types = NULL;
    int argc = ZEND_NUM_ARGS();
    int type_offset = 1;
    int type_count = argc - type_offset;
    int i, len = 0;

    if (zend_parse_parameters(1 TSRMLS_CC, (char *)"s", &name, &len) == FAILURE ||
        get_varargs(argc, &types) == 0) {
        if (types != NULL) {
            efree(types);
        }
        RETURN_FALSE;
    }

    zval *zobj = getThis();
    voltclient_object *obj = (voltclient_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    if (obj->procedures.find(name) == obj->procedures.end()) {
        std::vector<voltdb::Parameter> paramTypes(type_count);
        for (i = type_offset; i < argc; i++) {
            if (Z_TYPE_PP(types[i]) == IS_LONG) {
                paramTypes[i - type_offset] = voltdb::Parameter((voltdb::WireType)Z_LVAL_PP(types[i]));
            } else {
                efree(types);
                RETURN_FALSE;
            }
        }

        voltdb::Procedure *proc = new voltdb::Procedure(name, paramTypes);
        obj->procedures[name] = proc;

        efree(types);
        RETURN_TRUE;
    } else {
        efree(types);
        RETURN_FALSE;
    }
}

PHP_METHOD(VoltClient, removeStoredProc)
{
    char *name = NULL;
    int len = 0;

    RETURN_TRUE;
}

PHP_METHOD(VoltClient, getStoredProcs)
{

}

PHP_METHOD(VoltClient, invoke)
{
    zval *zobj = getThis();
    voltclient_object *obj = (voltclient_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    voltdb::Procedure *proc = prepare_to_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj);
    if (proc == NULL) {
        RETURN_NULL();
    }

    // Invoke the procedure
    voltdb::errType err = voltdb::errOk;
    voltdb::InvocationResponse resp = obj->client.invoke(err, *proc);
    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_NULL();
    }

    // Check if the response was constructed correctly
    err = resp.getErr();
    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_NULL();
    }

    // Instantiate a PHP response object
    struct voltresponse_object *ro = instantiate_voltresponse(return_value, resp);
    if (ro == NULL) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             voltdb::errException TSRMLS_CC);
        RETURN_NULL();
    }
}

PHP_METHOD(VoltClient, invokeAsync)
{

}

PHP_METHOD(VoltClient, drain)
{
    zval *zobj = getThis();
    voltclient_object *obj = (voltclient_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    voltdb::errType err = voltdb::errOk;

    bool retval = obj->client.drain(err);
    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_FALSE;
    }

    if (retval) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}
