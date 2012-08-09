/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

#include <map>
#include <vector>
#include "ConnectionPool.h"
#include "Client.h"
#include "Exception.hpp"
#include "Procedure.hpp"
#include "Parameter.hpp"
#include "WireType.h"
#include "php_voltdb.h"

// Initialize global variables
// ZEND_DECLARE_MODULE_GLOBALS(voltdb)

/* True global resources - no need for thread safety here */
static int le_voltdb;

// class entries used to instantiate the PHP classes
zend_class_entry *voltclient_ce;

/* {{{ voltdb_functions[]
 *
 * Every user visible function must have an entry in voltdb_functions[].
 */
const zend_function_entry voltdb_methods[] = {
    // VoltClient
    PHP_ME(VoltClient, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(VoltClient, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(VoltClient, connect, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, addStoredProc, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, removeStoredProc, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, getStoredProcs, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invoke, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invokeAsync, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, drain, NULL, ZEND_ACC_PUBLIC)

    // // VoltProcedure
    // PHP_ME(VoltProcedure, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltProcedure, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltProcedure, getParams, NULL, ZEND_ACC_PUBLIC)

    // // VoltInvocationResponse
    // PHP_ME(VoltInvocationResponse, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltInvocationResponse, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltInvocationResponse, statusCode, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltInvocationResponse, statusString, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltInvocationResponse, appStatusCode, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltInvocationResponse, appStatusString, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltInvocationResponse, results, NULL, ZEND_ACC_PUBLIC)

    // // VoltTable
    // PHP_ME(VoltTable, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltTable, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    // PHP_ME(VoltTable, iterator, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltTable, statusCode, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltTable, rowCount, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltTable, columnCount, NULL, ZEND_ACC_PUBLIC)
    // PHP_ME(VoltTable, columns, NULL, ZEND_ACC_PUBLIC)

	PHP_FE_END	/* Must be the last line in voltdb_functions[] */
};
/* }}} */

/* {{{ voltdb_module_entry
 */
zend_module_entry voltdb_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"voltdb",
	NULL,                       // Functions
	PHP_MINIT(voltdb),
	PHP_MSHUTDOWN(voltdb),
	PHP_RINIT(voltdb),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(voltdb),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(voltdb),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_VOLTDB
extern "C" {
ZEND_GET_MODULE(voltdb)
}
#endif

/* Common functions */

/* Get varargs into an array. The caller must efree() the args. */
static int get_varargs(int argc, zval ****args)
{
    if (argc < 1) {
        return 0;
    }

    *args = (zval ***)emalloc(argc * sizeof(zval **));
    php_printf("parsing varargs\n");
    if (zend_get_parameters_array_ex(argc, *args) == FAILURE) {
        php_printf("failed parsing varargs\n");
        efree(*args);
        return 0;
    }

    php_printf("parsed varargs\n");
    return 1;
}

/* VoltClient */
zend_object_handlers voltclient_object_handlers;
struct voltclient_object {
    zend_object std;
    voltdb::Client client;
    std::map<const char *, voltdb::Procedure *> procedures;
};

void voltclient_free(void *obj TSRMLS_CC)
{
    voltclient_object *client_obj = (voltclient_object *)obj;

    std::map<const char *, voltdb::Procedure *>::iterator it;
    for (it = client_obj->procedures.begin();
         it != client_obj->procedures.end();
         it++) {
        delete (*it).second;
    }
    client_obj->procedures.clear();

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

PHP_METHOD(VoltClient, __construct)
{
    // No-op, client is created in connect from the ConnectionPool
}

PHP_METHOD(VoltClient, __destruct)
{
    // TODO: Clean up the procedures
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

    if (zend_parse_parameters(argc TSRMLS_CC, "sss|l", &hostname, &len, &username, &len,
                              &password, &len, &port) == FAILURE) {
        RETURN_FALSE;
    }

    if (argc == 4) {
        obj->client = voltdb::ConnectionPool::pool()->acquireClient(err, hostname, username, password, port);
    } else {
        obj->client = voltdb::ConnectionPool::pool()->acquireClient(err, hostname, username, password);
    }

    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "", err TSRMLS_CC);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(VoltClient, addStoredProc)
{
    char *name = NULL;
    zval ***types = NULL;
    int type_offset = 1;
    int type_len = ZEND_NUM_ARGS() - type_offset;
    int i, len = 0;

    if (zend_parse_parameters(1 TSRMLS_CC, "s", &name, &len) == FAILURE ||
        get_varargs(type_len, &types) == 0) {
        RETURN_FALSE;
    }

    zval *zobj = getThis();
    voltclient_object *obj = (voltclient_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    if (obj->procedures.find(name) == obj->procedures.end()) {
        std::vector<voltdb::Parameter> paramTypes(type_len);
        for (i = type_offset; i < type_len; i++) {
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

}

PHP_METHOD(VoltClient, invokeAsync)
{

}

PHP_METHOD(VoltClient, drain)
{
    RETURN_TRUE;
}

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("voltdb.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_voltdb_globals, voltdb_globals)
    STD_PHP_INI_ENTRY("voltdb.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_voltdb_globals, voltdb_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_voltdb_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_voltdb_init_globals(zend_voltdb_globals *voltdb_globals)
{
	voltdb_globals->global_value = 0;
	voltdb_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(voltdb)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltClient", voltdb_methods);
    voltclient_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltclient_ce->create_object = voltclient_create_handler;
    memcpy(&voltclient_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    voltclient_object_handlers.clone_obj = NULL;

    // Initialize the connection pool
    voltdb::onLoad();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(voltdb)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(voltdb)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(voltdb)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(voltdb)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "voltdb support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
