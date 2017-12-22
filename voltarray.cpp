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

#include <vector>

#include "WireType.h"

#include "voltarray.h"
#include "exception.h"

// class entry used to instantiate the PHP array class
zend_class_entry *voltarray_ce;

const zend_function_entry voltarray_methods[] = {
    PHP_ME(VoltArray, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(VoltArray, add, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltArray, dump, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                  // Must be the last line
};

static zend_object_handlers voltarray_object_handlers;

PHP_VOLTDB_FREE_WRAPPED_FUNC_START(voltarray_object)
    // Free custom resources
    for (std::vector<zval>::iterator it = wrapped_obj->values.begin(); it != wrapped_obj->values.end(); it++) {
        zval_dtor(&(*it));
    }
    wrapped_obj->values.clear();
    std::vector<php_voltdb_zval_vec>().swap(wrapped_obj->values);
PHP_VOLTDB_FREE_WRAPPED_FUNC_END()

static php_voltdb_zend_object voltarray_create_handler(zend_class_entry *ce TSRMLS_DC)
{
    PHP_VOLTDB_ALLOC_CLASS_OBJECT(voltarray_object, ce)

    // Initialize the std object
    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    PHP_VOLTDB_INIT_CLASS_OBJECT_PROPERTIES()

    // Put the internal object into the object store
    // Assign the customized object storage free callback
    // Assign the customized object handlers
    PHP_VOLTDB_FREE_CLASS_OBJECT(voltarray_object, voltarray_object_handlers)
}

void create_voltarray_class(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltArray", voltarray_methods);
    voltarray_ce = zend_register_internal_class(&ce TSRMLS_CC);
    voltarray_ce->create_object = voltarray_create_handler;

    PHP_VOLTDB_INIT_HANDLER(voltarray_object, voltarray_object_handlers)

    // Set up all status codes as class constants
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_TINYINT", 17,
                                     voltdb::WIRE_TYPE_TINYINT TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_SMALLINT", 18,
                                     voltdb::WIRE_TYPE_SMALLINT TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_INTEGER", 17,
                                     voltdb::WIRE_TYPE_INTEGER TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_BIGINT", 16,
                                     voltdb::WIRE_TYPE_BIGINT TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_FLOAT", 15,
                                     voltdb::WIRE_TYPE_FLOAT TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_STRING", 16,
                                     voltdb::WIRE_TYPE_STRING TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_TIMESTAMP", 19,
                                     voltdb::WIRE_TYPE_TIMESTAMP TSRMLS_CC);
    zend_declare_class_constant_long(voltarray_ce,
                                     "VOLT_TYPE_DECIMAL", 17,
                                     voltdb::WIRE_TYPE_DECIMAL TSRMLS_CC);
}

PHP_METHOD(VoltArray, __construct)
{
    zval *zobj = getThis();
    voltarray_object *obj = Z_VOLTARRAY_OBJECT_P(zobj);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (char *)"l", &obj->wire_type) == FAILURE) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "VoltArray expects a long", errException TSRMLS_CC);
    }
}

PHP_METHOD(VoltArray, add)
{
    zval *zobj = getThis();
    voltarray_object *obj = Z_VOLTARRAY_OBJECT_P(zobj);

    int argc = ZEND_NUM_ARGS();
    zval *args = (zval *)safe_emalloc(argc, sizeof(zval), 0);
    if (argc == 0 || zend_get_parameters_array_ex(argc, args) == FAILURE) {
        efree(args);
        WRONG_PARAM_COUNT;
    }
    for (int i = 0; i < argc; i++) {
        if (Z_TYPE(args[i]) == IS_ARRAY) {
            HashTable *arr = Z_ARRVAL(args[i]);
            zval *entry = NULL;
            PHP_VOLTDB_HASH_FOREACH_VAL_START(arr, entry)
                obj->values.push_back(*entry);
                ZVAL_COPY_VALUE(&obj->values.back(), entry);
            PHP_VOLTDB_HASH_FOREACH_END()
        }
        else {
            obj->values.push_back(args[i]);
            ZVAL_COPY_VALUE(&obj->values.back(), &args[i]);
        }
    }
    efree(args);
    RETURN_NULL();
}

PHP_METHOD(VoltArray, dump)
{
    zval *zobj = getThis();
    voltarray_object *obj = Z_VOLTARRAY_OBJECT_P(zobj);

    RETVAL_LONG(obj->wire_type);
    php_var_dump(return_value, 1);
    for (std::vector<zval>::iterator it = obj->values.begin(); it != obj->values.end(); it++) {
        php_var_dump(&(*it), 1);
    }
    RETURN_NULL();
}
