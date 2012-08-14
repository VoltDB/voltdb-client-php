/* This file is part of VoltDB.
 * Copyright (C) 2008-2012 VoltDB Inc.
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

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

#include "InvocationResponse.hpp"
#include "table.h"
#include "response.h"

// class entry used to instantiate the PHP response class
zend_class_entry *voltresponse_ce;

const zend_function_entry voltresponse_methods[] = {
    PHP_ME(VoltInvocationResponse, statusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, statusString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, appStatusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, appStatusString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, hasMoreResults, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltInvocationResponse, nextResult, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                  // Must be the last line
};

zend_object_handlers voltresponse_object_handlers;

void voltresponse_free(void *obj TSRMLS_CC)
{
    voltresponse_object *response_obj = (voltresponse_object *)obj;

    delete response_obj->response;

    zend_hash_destroy(response_obj->std.properties);
    FREE_HASHTABLE(response_obj->std.properties);

    efree(response_obj);
}

zend_object_value voltresponse_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    voltresponse_object *obj = (voltresponse_object *)emalloc(sizeof(voltresponse_object));
    memset(obj, 0, sizeof(voltresponse_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
                                           voltresponse_free, NULL TSRMLS_CC);
    retval.handlers = &voltresponse_object_handlers;

    return retval;
}

void create_voltresponse_class(void)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltInvocationResponse", voltresponse_methods);
    voltresponse_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltresponse_ce->create_object = voltresponse_create_handler;
    memcpy(&voltresponse_object_handlers,
           zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));
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

struct voltresponse_object *instantiate_voltresponse(zval *return_val,
                                                     voltdb::InvocationResponse &resp)
{
    struct voltresponse_object *ro = NULL;

    if (object_init_ex(return_val, voltresponse_ce) != SUCCESS) {
        return NULL;
    }

    ro = (struct voltresponse_object *)zend_object_store_get_object(return_val TSRMLS_CC);
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

    voltdb::errType err = table.getErr();
    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_NULL();
    }

    // Wrap the table in a PHP class
    struct volttable_object *to = instantiate_volttable(return_value, table);
    if (to == NULL) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             voltdb::errException TSRMLS_CC);
        RETURN_NULL();
    }
}
