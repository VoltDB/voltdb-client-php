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

#include "Table.h"

#include "common.h"
#include "volttable.h"
#include "exception.h"

// class entry used to instantiate the PHP table class
zend_class_entry *volttable_ce;

const zend_function_entry volttable_methods[] = {
    PHP_ME(VoltTable, statusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, rowCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, columnCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, hasMoreRows, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, nextRow, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                  // Must be the last line
};

static zend_object_handlers volttable_object_handlers;

static void volttable_free_object_storage_handler(volttable_object *table_obj TSRMLS_DC)
{
    // Free the std object
    zend_object_std_dtor(&table_obj->std TSRMLS_CC);

    // Free additional resources
    delete table_obj->table;
    table_obj->table = NULL;

    efree(table_obj);
}

static zend_object_value volttable_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    volttable_object *obj = (volttable_object *)emalloc(sizeof(volttable_object));
    memset(obj, 0, sizeof(volttable_object));

    // Initialize the std object
    zend_object_std_init(&obj->std, type TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &type->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
#else
    object_properties_init(&(obj->std), type);
#endif

    // Put the internal object into the object store
    retval.handle = zend_objects_store_put(
        obj,
        NULL,
        (zend_objects_free_object_storage_t) volttable_free_object_storage_handler,
        NULL TSRMLS_CC);

    // Assign the customized object handlers
    retval.handlers = &volttable_object_handlers;

    return retval;
}

void create_volttable_class(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltTable", volttable_methods);
    volttable_ce = zend_register_internal_class(&ce TSRMLS_CC);
    volttable_ce->create_object = volttable_create_handler;

    // Create customized object handlers
    volttable_object_handlers = *zend_get_std_object_handlers();
    volttable_object_handlers.clone_obj = NULL;
}

volttable_object *instantiate_volttable(zval *return_val, voltdb::Table &table)
{
    volttable_object *to = NULL;

    if (object_init_ex(return_val, volttable_ce) != SUCCESS) {
        return NULL;
    }

    to = (volttable_object *)zend_object_store_get_object(return_val TSRMLS_CC);
    assert(to != NULL);
    to->table = new voltdb::Table(table);
    to->it = to->table->iterator();

    return to;
}

static int row_to_array(zval *return_value, voltdb::Row row)
{
    int count = row.columnCount();
    std::vector<voltdb::Column> columns = row.columns();

    for (int i = 0; i < count; i++) {
        std::string name = columns[i].name();
        int name_len = name.length() + 1; // including the terminator

        bool isNull;
        try {
            isNull = row.isNull(i);
        } catch (voltdb::InvalidColumnException) {
            return 0;
        }

        if (isNull) {
            add_assoc_null_ex(return_value, name.c_str(), name_len);
        } else {
            switch (columns[i].type()) {
            case voltdb::WIRE_TYPE_TINYINT:
            {
                int8_t value;
                try {
                    value = row.getInt8(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_long_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_SMALLINT:
            {
                int16_t value;
                try {
                    value = row.getInt16(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_long_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_INTEGER:
            {
                int32_t value;
                try {
                    value = row.getInt32(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_long_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_BIGINT:
            {
                int64_t value;
                try {
                    value = row.getInt64(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_long_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_FLOAT:
            {
                double value;
                try {
                    value = row.getDouble(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_double_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_STRING:
            {
                std::string value;
                try {
                    value = row.getString(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                /*
                 * necessary to dup here because the add_assoc_string_ex takes
                 * char*. No need to free it, PHP should take care of this when
                 * the refcount is decremented.
                 */
                char *dup_val = estrdup(value.c_str());
                add_assoc_string_ex(return_value, name.c_str(), name_len, dup_val, 0);
                break;
            }
            case voltdb::WIRE_TYPE_TIMESTAMP:
            {
                int64_t value;
                try {
                    value = row.getTimestamp(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                add_assoc_long_ex(return_value, name.c_str(), name_len, value);
                break;
            }
            case voltdb::WIRE_TYPE_DECIMAL:
            {
                voltdb::Decimal value;
                try {
                    value = row.getDecimal(i);
                } catch (voltdb::InvalidColumnException) {
                    return 0;
                }
                /*
                 * return decimal as a string. PHP float doesn't have enough
                 * precision to hold a SQL decimal
                 */
                char *dup_val = estrdup(value.toString().c_str());
                add_assoc_string_ex(return_value, name.c_str(), name_len, dup_val, 0);
                break;
            }
            case voltdb::WIRE_TYPE_VARBINARY:
            {
                // TODO: encode it and store it as a string?
                break;
            }
            default:
                return 0;
            }
        }
    }

    return 1;
}

PHP_METHOD(VoltTable, statusCode)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->table->getStatusCode());
}

PHP_METHOD(VoltTable, rowCount)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->table->rowCount());
}

PHP_METHOD(VoltTable, columnCount)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    RETURN_LONG(obj->table->columnCount());
}

PHP_METHOD(VoltTable, hasMoreRows)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    if (!obj->it.hasNext()) {
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

PHP_METHOD(VoltTable, nextRow)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);

    if (!obj->it.hasNext()) {
        RETURN_NULL();
    }

    // Get the next row and advance the iterator
    try {
        voltdb::Row row = obj->it.next();

        // Convert the row into a PHP array
        if (array_init(return_value) == FAILURE) {
            zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                                 errException TSRMLS_CC);
            RETURN_NULL();
        }

        if (!row_to_array(return_value, row)) {
            RETURN_NULL();
        }
    } catch (voltdb::NoMoreRowsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errNoMoreRowsException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::OverflowUnderflowException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errOverflowUnderflowException TSRMLS_CC);
        RETURN_NULL();
    } catch (voltdb::IndexOutOfBoundsException) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, errIndexOutOfBoundsException TSRMLS_CC);
        RETURN_NULL();
    }
}
