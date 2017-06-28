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

PHP_VOLTDB_FREE_WRAPPED_FUNC_START(voltresponse_object)
    // Free custom resources
    wrapped_obj->results.clear();
    std::vector<voltdb::Table>().swap(wrapped_obj->results);
    if (wrapped_obj->response != NULL) {
        wrapped_obj->response->~InvocationResponse();
        efree(wrapped_obj->response);
        wrapped_obj->response = NULL;
    }
PHP_VOLTDB_FREE_WRAPPED_FUNC_END()

static php_voltdb_zend_object voltresponse_create_handler(zend_class_entry *ce TSRMLS_DC)
{
    PHP_VOLTDB_ALLOC_CLASS_OBJECT(voltresponse_object, ce)

    // Initialize the std object
    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    PHP_VOLTDB_INIT_CLASS_OBJECT_PROPERTIES()

    // Put the internal object into the object store
    // Assign the customized object storage free callback
    // Assign the customized object handlers
    PHP_VOLTDB_FREE_CLASS_OBJECT(voltresponse_object, voltresponse_object_handlers)
}

void create_voltresponse_class(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltInvocationResponse", voltresponse_methods);
    voltresponse_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltresponse_ce->create_object = voltresponse_create_handler;

    PHP_VOLTDB_INIT_HANDLER(voltresponse_object, voltresponse_object_handlers)

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
                                              voltdb::InvocationResponse *resp TSRMLS_DC)
{
    voltresponse_object *ro = NULL;

    if (object_init_ex(return_val, voltresponse_ce) != SUCCESS) {
        return NULL;
    }

    ro = Z_VOLTRESPONSE_OBJECT_P(return_val);
    assert(ro != NULL);
    ro->response = resp;
    ro->results = ro->response->results();
    ro->it = ro->results.begin();

    return ro;
}

PHP_METHOD(VoltInvocationResponse, statusCode)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    RETURN_LONG(obj->response->statusCode());
}

PHP_METHOD(VoltInvocationResponse, statusString)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    // The second parameter 1 tells PHP to make a copy of the string
    PHP_VOLTDB_RETURN_STRING(obj->response->statusString().c_str(), 1);
}

PHP_METHOD(VoltInvocationResponse, appStatusCode)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    RETURN_LONG(obj->response->appStatusCode());
}

PHP_METHOD(VoltInvocationResponse, appStatusString)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    // The second parameter 1 tells PHP to make a copy of the string
    PHP_VOLTDB_RETURN_STRING(obj->response->appStatusString().c_str(), 1);
}

PHP_METHOD(VoltInvocationResponse, resultCount)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    RETURN_LONG(obj->results.size());
}

PHP_METHOD(VoltInvocationResponse, hasMoreResults)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);
    if (obj->it < obj->results.end()) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}

PHP_METHOD(VoltInvocationResponse, nextResult)
{
    zval *zobj = getThis();
    voltresponse_object *obj = Z_VOLTRESPONSE_OBJECT_P(zobj);

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
