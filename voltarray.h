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

#ifndef VOLT_ARRAY_H
#define VOLT_ARRAY_H

#include "common.h"

extern zend_class_entry *voltarray_ce;

// VoltArray
PHP_METHOD(VoltArray, __construct);
PHP_METHOD(VoltArray, add);
PHP_METHOD(VoltArray, dump);

PHP_VOLTDB_WRAP_OBJECT_START(voltarray_object)
    php_voltdb_long wire_type;
    std::vector<php_voltdb_zval_vec> values;
PHP_VOLTDB_WRAP_OBJECT_END(voltarray_object)

#if PHP_MAJOR_VERSION < 7

#define Z_VOLTARRAY_OBJECT_P(zv) \
  (voltarray_object *)zend_object_store_get_object(zv TSRMLS_CC)

#else

static inline voltarray_object
*voltarray_object_from_obj(zend_object *obj) {
    return (voltarray_object *)((char*)(obj) -
                                   XtOffsetOf(voltarray_object, std));
}

#define Z_VOLTARRAY_OBJECT_P(zv) voltarray_object_from_obj(Z_OBJ_P((zv)))

#endif /* PHP_MAJOR_VERSION */

void create_voltarray_class(TSRMLS_D);

#endif  // VOLT_ARRAY_H
