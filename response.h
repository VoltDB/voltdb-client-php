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

#ifndef VOLT_INVOCATION_RESPONSE_H
#define VOLT_INVOCATION_RESPONSE_H

#include "common.h"

// VoltInvocationResponse
PHP_METHOD(VoltInvocationResponse, statusCode);
PHP_METHOD(VoltInvocationResponse, statusString);
PHP_METHOD(VoltInvocationResponse, appStatusCode);
PHP_METHOD(VoltInvocationResponse, appStatusString);
PHP_METHOD(VoltInvocationResponse, resultCount);
PHP_METHOD(VoltInvocationResponse, hasMoreResults);
PHP_METHOD(VoltInvocationResponse, nextResult);

PHP_VOLTDB_WRAP_OBJECT_START(voltresponse_object)
    voltdb::InvocationResponse *response;
    std::vector<voltdb::Table> results;
    std::vector<voltdb::Table>::const_iterator it;
PHP_VOLTDB_WRAP_OBJECT_END(voltresponse_object)

#if PHP_MAJOR_VERSION < 7

#define Z_VOLTRESPONSE_OBJECT_P(zv) \
  (voltresponse_object *)zend_object_store_get_object(zv TSRMLS_CC)

#else

static inline voltresponse_object
*voltresponse_object_from_obj(zend_object *obj) {
    return (voltresponse_object *)((char*)(obj) -
                                   XtOffsetOf(voltresponse_object, std));
}

#define Z_VOLTRESPONSE_OBJECT_P(zv) voltresponse_object_from_obj(Z_OBJ_P((zv)))

#endif /* PHP_MAJOR_VERSION */

void create_voltresponse_class(TSRMLS_D);
voltresponse_object *instantiate_voltresponse(zval *return_val,
                                              voltdb::InvocationResponse *resp TSRMLS_DC);

#endif  // VOLT_INVOCATION_RESPONSE_H
