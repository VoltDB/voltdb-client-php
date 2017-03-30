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

#include "InvocationResponse.hpp"

#include "common.h"
#include "volttable.h"
#include "response.h"
#include "exception.h"

// class entry used to instantiate the PHP response class
zend_class_entry *voltresponse_ce;

const zend_function_entry voltresponse_methods[] = {
    PHP_ME(VoltInvocationResponse, statusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, statusString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, appStatusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, appStatusString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, resultCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, hasMoreResults, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, nextResult, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                  // Must be the last line
};

static zend_object_handlers voltresponse_object_handlers;

static void voltresponse_free_object_storage_handler(voltresponse_object *response_obj TSRMLS_DC)
{
    // Free the std object
    zend_object_std_dtor(&response_obj->std TSRMLS_CC);

    // Free additional resources
    response_obj->results.clear();
    delete response_obj->response;
    response_obj->response = NULL;

    efree(response_obj);
}

static zend_object_value voltresponse_create_handler(zend_class_entry *ce TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    voltresponse_object *obj = (voltresponse_object *)emalloc(sizeof(voltresponse_object));
    memset(obj, 0, sizeof(voltresponse_object));

    // Initialize the std object
    zend_object_std_init(&obj->std, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &ce->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
#else
    object_properties_init(&(obj->std), ce);
#endif

    // Put the internal object into the object store
    retval.handle = zend_objects_store_put(
        obj,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) voltresponse_free_object_storage_handler,
        NULL TSRMLS_CC);

    // Assign the customized object handlers
    retval.handlers = &voltresponse_object_handlers;

    return retval;
}

void create_voltresponse_class(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltInvocationResponse", voltresponse_methods);
    voltresponse_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltresponse_ce->create_object = voltresponse_create_handler;

    // Create customized object handlers
    voltresponse_object_handlers = *zend_get_std_object_handlers();
    voltresponse_object_handlers.clone_obj = NULL;

    // Set up all status codes as class constants
    zend_declare_class_constant_long(voltresponse_ce,
                                     "SUCCESS", 7,
                                     voltdb::STATUS_CODE_SUCCESS TSRMLS_CC);
    zend_declare_class_constant_long(voltresponse_ce,
                                     "USER_ABORT", 10,
                                     voltdb::STATUS_CODE_USER_ABORT TSRMLS_CC);
    zend_declare_class_constant_long(voltresponse_ce,
                                     "GRACEFUL_FAILURE", 16,
                                     voltdb::STATUS_CODE_GRACEFUL_FAILURE TSRMLS_CC);
    zend_declare_class_constant_long(voltresponse_ce,
                                     "UNEXPECTED_FAILURE", 18,
                                     voltdb::STATUS_CODE_UNEXPECTED_FAILURE TSRMLS_CC);
    zend_declare_class_constant_long(voltresponse_ce,
                                     "CONNECTION_LOST", 15,
                                     voltdb::STATUS_CODE_CONNECTION_LOST TSRMLS_CC);
}

voltresponse_object *instantiate_voltresponse(zval *return_val,
                                              voltdb::InvocationResponse &resp TSRMLS_DC)
{
    voltresponse_object *ro = NULL;

    if (object_init_ex(return_val, voltresponse_ce) != SUCCESS) {
        return NULL;
    }

    ro = (voltresponse_object *)zend_object_store_get_object(return_val TSRMLS_CC);
    assert(ro != NULL);
    ro->response = new voltdb::InvocationResponse(resp);
    ro->results = ro->response->results();
    ro->it = ro->results.begin();

    return ro;
}

PHP_METHOD(VoltInvocationResponse, statusCode)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->response->statusCode());
}

PHP_METHOD(VoltInvocationResponse, statusString)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    // The second parameter 1 tells PHP to make a copy of the string
    RETURN_STRING(obj->response->statusString().c_str(), 1);
}

PHP_METHOD(VoltInvocationResponse, appStatusCode)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->response->appStatusCode());
}

PHP_METHOD(VoltInvocationResponse, appStatusString)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    // The second parameter 1 tells PHP to make a copy of the string
    RETURN_STRING(obj->response->appStatusString().c_str(), 1);
}

PHP_METHOD(VoltInvocationResponse, resultCount)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->results.size());
}

PHP_METHOD(VoltInvocationResponse, hasMoreResults)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    if (obj->it < obj->results.end()) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}

PHP_METHOD(VoltInvocationResponse, nextResult)
{
    zval *zobj = getThis();
    voltresponse_object *obj = (voltresponse_object *)zend_object_store_get_object(zobj TSRMLS_CC);

    if (obj->it >= obj->results.end()) {
        RETURN_NULL();
    }

    // Get the table and advance the iterator
    voltdb::Table table = *(obj->it);
    obj->it++;

    // Wrap the table in a PHP class
    volttable_object *to = instantiate_volttable(return_value, table TSRMLS_CC);
    if (to == NULL) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             errException TSRMLS_CC);
        RETURN_NULL();
    }
}
