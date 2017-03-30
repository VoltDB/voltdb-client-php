/* This file is part of VoltDB.
 * Copyright (C) 2008-2017 VoltDB Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

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
#include "ProcedureCallback.hpp"
#include "StatusListener.h"

#include "response.h"
#include "client.h"
#include "exception.h"

static const char *VOLT_NULL_INDICATOR = "\\N";

// class entry used to instantiate the PHP client class
zend_class_entry *voltclient_ce;

// Identifier for the response resource
static int le_voltresponse;

const zend_function_entry voltclient_methods[] = {
    // VoltClient
    PHP_ME(VoltClient, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(VoltClient, connect, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invoke, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, invokeAsync, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, getResponse, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, drain, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltClient, close, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                /* Must be the last line in zend_function_entry */
};

/*
 * A status listener that tells the the client to not block on backpressure. The
 * client will queue invocations until backpressure is relieved. This status
 * listener is used for all client creations.
 */
class StatusListener: public voltdb::StatusListener {
    bool uncaughtException(std::exception exception,
                           boost::shared_ptr<voltdb::ProcedureCallback> callback,
                           voltdb::InvocationResponse response) {
        return true;
    }

    bool connectionLost(std::string hostname, int32_t connectionsLeft) {
        return true;
    }

    bool connectionActive(std::string hostname, int32_t connectionsActive) {
        return true;
    }

    bool backpressure(bool hasBackpressure) {
        // This unblocks invoke() on backpressure
        return true;
    }
};
// Cache the status listener
static StatusListener status_listener;

class VoltCallback : public voltdb::ProcedureCallback {
public:
    VoltCallback(php_voltdb_zend_resource_id rsrc_id TSRMLS_DC) {
        m_rsrc_id = rsrc_id;
#if PHP_MAJOR_VERSION < 7 && defined(ZTS)
        this->TSRMLS_C = TSRMLS_C;
#endif
        /*
         * Increment the ref count so that the resource won't be deleted before
         * the callback is called
         */
        PHP_VOLTDB_LIST_ADD_REF(m_rsrc_id);
    }

    /*
     * Does not actually throw an exception, but needs to match the definition of
     * callback in voltdb::ProcedureCallback
     */
    bool callback(voltdb::InvocationResponse response) throw (voltdb::Exception) {
        PHP_VOLTDB_LIST_FIND(voltresponse_res, m_rsrc_id)

        if (ptr == NULL || type != le_voltresponse) {
            // TODO: shouldn't happen
        } else {
            ptr->resp = new voltdb::InvocationResponse(response);
        }

        /*
         * Decrement the ref count on the resource so it can be deleted if
         * nobody will access it.
         */
        zend_list_delete(m_rsrc_id);

        return false;
    }
private:
    php_voltdb_zend_resource_id m_rsrc_id;
#if PHP_MAJOR_VERSION < 7 && defined(ZTS)
    TSRMLS_D;
#endif
};

static void voltresponse_dtor(php_voltdb_zend_resource *rsrc TSRMLS_DC)
{
    voltresponse_res *res = (voltresponse_res *)rsrc->ptr;
    assert(res != NULL);

    if (res->resp != NULL) {
        delete res->resp;
        res->resp = NULL;
    }
    efree(res);
}

/* VoltClient */
static zend_object_handlers voltclient_object_handlers;

PHP_VOLTDB_FREE_WRAPPED_FUNC_START(voltclient_object)
    // Free custom resources
    std::map<const char *, voltdb::Procedure *>::iterator it;
    for (it = wrapped_obj->procedures.begin();
         it != wrapped_obj->procedures.end();
         it++) {
        efree((void *)it->first);
        delete it->second;
    }
    wrapped_obj->procedures.clear();

    delete wrapped_obj->client;
    wrapped_obj->client = NULL;

    // Return the client held by this thread to the pool
    voltdb::ConnectionPool::pool()->onScriptEnd();
PHP_VOLTDB_FREE_WRAPPED_FUNC_END()

static php_voltdb_zend_object voltclient_create_handler(zend_class_entry *ce TSRMLS_DC)
{
    PHP_VOLTDB_ALLOC_CLASS_OBJECT(voltclient_object, ce)

    // Initialize the std object
    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    PHP_VOLTDB_INIT_CLASS_OBJECT_PROPERTIES()

    // Initialize other resources
    intern->procedures = std::map<const char *, voltdb::Procedure *>();

    // Put the internal object into the object store
    // Assign the customized object storage free callback
    // Assign the customized object handlers
    PHP_VOLTDB_FREE_CLASS_OBJECT(voltclient_object, voltclient_object_handlers)
}

void create_voltclient_class(int module_number TSRMLS_DC)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltClient", voltclient_methods);
    voltclient_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltclient_ce->create_object = voltclient_create_handler;

    PHP_VOLTDB_INIT_HANDLER(voltclient_object, voltclient_object_handlers)

    // Register a destructor for the response resource
    le_voltresponse = zend_register_list_destructors_ex(voltresponse_dtor, NULL,
                                                        (char *)VOLT_RESPONSE_RES_NAME,
                                                        module_number);
}

voltdb::Procedure *get_procedure(voltclient_object *obj, const char *name, int param_count)
{
    std::map<const char *, voltdb::Procedure *>::const_iterator it = obj->procedures.find(name);
    voltdb::Procedure *proc = NULL;
    int i = 0;

    if (it == obj->procedures.end()) {
        // Procedure doesn't exist, create one with all string params
        std::vector<voltdb::Parameter> paramTypes(param_count);
        for (i = 0; i < param_count; i++) {
            paramTypes[i] = voltdb::Parameter(voltdb::WIRE_TYPE_STRING);
        }

        name = estrdup(name);
        proc = new voltdb::Procedure(name, paramTypes);
        obj->procedures[name] = proc;
    } else {
        proc = it->second;
    }

    return proc;
}

voltdb::Procedure *prepare_to_invoke(INTERNAL_FUNCTION_PARAMETERS, voltclient_object *obj)
{
    char *name = NULL;
    zval *params = NULL;
    zval *param = NULL;
    HashTable *param_hash;
    HashPosition param_ptr;
    int param_count = 0;
    php_voltdb_int len = 0;

    assert(obj != NULL);

    // Check if the client is created.
    if (obj->client == NULL) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             errConnectException TSRMLS_CC);
        return NULL;
    }

    // Get the procedure name and parameters
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (char *)"s|a", &name, &len, &params) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             errParamMismatchException TSRMLS_CC);
        return NULL;
    }
    if (params != NULL) {
        param_hash = Z_ARRVAL_P(params);
        param_count = zend_hash_num_elements(param_hash);
    }

    // Get the procedure
    voltdb::Procedure *proc = get_procedure(obj, name, param_count);
    voltdb::ParameterSet *proc_params = proc->params();
    try {
        proc_params->reset();
    } catch (voltdb::OverflowUnderflowException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             errOverflowUnderflowException TSRMLS_CC);
        return NULL;
    }

    if (params != NULL) {
        // Set the parameters, only string params
        PHP_VOLTDB_HASH_FOREACH_VAL_START(param_hash, param)
            switch (Z_TYPE_P(param)) {
            case IS_NULL:
                try {
                    proc_params->addString(VOLT_NULL_INDICATOR);
                } catch (voltdb::ParamMismatchException) {
                    zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                         errParamMismatchException TSRMLS_CC);
                    return NULL;
                }
                break;
            case IS_STRING:
                try {
                    proc_params->addString(std::string(Z_STRVAL_P(param), Z_STRLEN_P(param)));
                } catch (voltdb::ParamMismatchException) {
                    zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                         errParamMismatchException TSRMLS_CC);
                    return NULL;
                }
                break;
            case IS_LONG:
            case IS_DOUBLE:
#if PHP_MAJOR_VERSION < 7
            case IS_BOOL:
#else
            case IS_FALSE:
            case IS_TRUE:
#endif
            case IS_CONSTANT:
                // Other primitive types will be converted to string
                /*
                 * Necessary to make a copy of the array element before
                 * converting the value to string, otherwise the original value
                 * will be converted.
                 */
                zval temp;
                temp = *param;
                zval_copy_ctor(&temp);
                convert_to_string(&temp);
                try {
                    proc_params->addString(std::string(Z_STRVAL(temp), Z_STRLEN(temp)));
                    zval_dtor(&temp);
                } catch (voltdb::ParamMismatchException) {
                    zval_dtor(&temp);
                    zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                         errParamMismatchException TSRMLS_CC);
                    return NULL;
                }
                break;
            default:
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                     errParamMismatchException TSRMLS_CC);
                return NULL;
            }
        PHP_VOLTDB_HASH_FOREACH_END()
    }

    return proc;
}

PHP_METHOD(VoltClient, __construct)
{
    // No-op, client is created in connect from the ConnectionPool
}

PHP_METHOD(VoltClient, connect)
{
    int argc = ZEND_NUM_ARGS();
    php_voltdb_int len = 0;
    char *hostname = NULL;
    char *username = NULL;
    char *password = NULL;
    php_voltdb_long port = 21212;

    zval *zobj = getThis();
    voltclient_object *obj = Z_VOLTCLIENT_OBJECT_P(zobj);

    if (zend_parse_parameters(argc TSRMLS_CC, (char *)"sss|l", &hostname, &len, &username, &len,
                              &password, &len, &port) == FAILURE) {
        RETURN_FALSE;
    }

    try {
        obj->client = new voltdb::Client(voltdb::ConnectionPool::pool()->acquireClient(
                                             hostname, username,
                                             password, &status_listener,
                                             port, voltdb::HASH_SHA256));
    } catch (voltdb::ConnectException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errConnectException TSRMLS_CC);
        RETURN_FALSE;
    } catch (voltdb::LibEventException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errLibEventException TSRMLS_CC);
        RETURN_FALSE;
    } catch (voltdb::Exception) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errException TSRMLS_CC);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(VoltClient, invoke)
{
    zval *zobj = getThis();
    voltclient_object *obj = Z_VOLTCLIENT_OBJECT_P(zobj);

    voltdb::Procedure *proc = prepare_to_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj);
    if (proc == NULL) {
        RETURN_NULL();
    }

    // Invoke the procedure
    voltdb::InvocationResponse resp;
    try {
        resp = obj->client->invoke(*proc);
    } catch (voltdb::NoConnectionsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errNoConnectionsException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::UninitializedParamsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errUninitializedParamsException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::LibEventException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errLibEventException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::Exception) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errException TSRMLS_CC);
        RETURN_NULL();
    }

    // Instantiate a PHP response object
    voltresponse_object *ro = instantiate_voltresponse(return_value, resp TSRMLS_CC);
    if (ro == NULL) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             errException TSRMLS_CC);
        RETURN_NULL();
    }
}

PHP_METHOD(VoltClient, invokeAsync)
{
    zval *zobj = getThis();
    voltclient_object *obj = Z_VOLTCLIENT_OBJECT_P(zobj);

    voltdb::Procedure *proc = prepare_to_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj);
    if (proc == NULL) {
        RETURN_NULL();
    }

    // Set up the response resource and the callback
    voltresponse_res *response = (voltresponse_res *)emalloc(sizeof(voltresponse_res));
    php_voltdb_zend_resource_id rsrc_id;
    PHP_VOLTDB_REGISTER_RESOURCE(rsrc_id, return_value, response, le_voltresponse)
    boost::shared_ptr<VoltCallback> callback(new VoltCallback(rsrc_id TSRMLS_CC));

    // Invoke the procedure
    try {
        obj->client->invoke(*proc, callback);
    } catch (voltdb::NoConnectionsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errNoConnectionsException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::UninitializedParamsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errUninitializedParamsException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::LibEventException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errLibEventException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::Exception) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errException TSRMLS_CC);
        RETURN_NULL();
    }
}

PHP_METHOD(VoltClient, getResponse)
{
    voltresponse_res *response = NULL;
    zval *zresponse = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (char *)"r", &zresponse) == FAILURE) {
        RETURN_NULL();
    }

    PHP_VOLTDB_FETCH_RESOURCE(response, voltresponse_res, zresponse,
                              VOLT_RESPONSE_RES_NAME, le_voltresponse)
    if (response != NULL && response->resp != NULL) {
        // Instantiate a PHP response object
        voltresponse_object *ro = instantiate_voltresponse(return_value, *response->resp TSRMLS_CC);
        if (ro == NULL) {
            zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                 errException TSRMLS_CC);
            RETURN_NULL();
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(VoltClient, drain)
{
    zval *zobj = getThis();
    voltclient_object *obj = Z_VOLTCLIENT_OBJECT_P(zobj);

    bool retval;
    try {
        retval = obj->client->drain();
    } catch (voltdb::NoConnectionsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errNoConnectionsException TSRMLS_CC);
        RETURN_FALSE;
    } catch (voltdb::LibEventException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errLibEventException TSRMLS_CC);
        RETURN_FALSE;
    } catch (voltdb::Exception) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errException TSRMLS_CC);
        RETURN_FALSE;
    }

    if (retval) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}

PHP_METHOD(VoltClient, close)
{
    zval *zobj = getThis();
    voltclient_object *obj = Z_VOLTCLIENT_OBJECT_P(zobj);

    try {
        if (obj->client != NULL) {
            voltdb::ConnectionPool::pool()->closeClientConnection(*(obj->client));
            delete obj->client;
            obj->client = NULL;
        }
    } catch (voltdb::MisplacedClientException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errMisplacedClientException TSRMLS_CC);
        RETURN_NULL();
    }
}
