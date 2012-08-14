extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

#include "Table.h"
#include "table.h"

// class entry used to instantiate the PHP table class
zend_class_entry *volttable_ce;

const zend_function_entry volttable_methods[] = {
    PHP_ME(VoltTable, statusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, rowCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, columnCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, columnIndexFromName, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, hasMoreRows, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(VoltTable, nextRow, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END                  // Must be the last line
};

zend_object_handlers volttable_object_handlers;

void volttable_free(void *obj TSRMLS_CC)
{
    volttable_object *table_obj = (volttable_object *)obj;

    delete table_obj->table;

    zend_hash_destroy(table_obj->std.properties);
    FREE_HASHTABLE(table_obj->std.properties);

    efree(table_obj);
}

zend_object_value volttable_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    volttable_object *obj = (volttable_object *)emalloc(sizeof(volttable_object));
    memset(obj, 0, sizeof(volttable_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
                                           volttable_free, NULL TSRMLS_CC);
    retval.handlers = &volttable_object_handlers;

    return retval;
}

void create_volttable_class(void)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "VoltTable", volttable_methods);
    volttable_ce = zend_register_internal_class(&ce TSRMLS_CC);
    volttable_ce->create_object = volttable_create_handler;
    memcpy(&volttable_object_handlers,
           zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));
    volttable_object_handlers.clone_obj = NULL;
}

struct volttable_object *instantiate_volttable(zval *return_val, voltdb::Table &table)
{
    struct volttable_object *to = NULL;

    if (object_init_ex(return_val, volttable_ce) != SUCCESS) {
        return NULL;
    }

    to = (struct volttable_object *)zend_object_store_get_object(return_val TSRMLS_CC);
    assert(to != NULL);
    to->table = new voltdb::Table(table);
    to->it = to->table->iterator();

    return to;
}

int row_to_array(zval *return_value, voltdb::Row row)
{
    voltdb::errType err = voltdb::errOk;
    int count = row.columnCount();
    std::vector<voltdb::Column> columns = row.columns();

    for (int i = 0; i < count; i++) {
        bool isNull = row.isNull(err, i);
        if (!voltdb::isOk(err)) {
            // TODO: Should we free the array?
            return 0;
        }

        if (isNull) {
            add_next_index_null(return_value);
        } else {
            switch (columns[i].type()) {
            case voltdb::WIRE_TYPE_TINYINT:
            {
                int8_t value = row.getInt8(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_long(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_SMALLINT:
            {
                int16_t value = row.getInt16(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_long(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_INTEGER:
            {
                int32_t value = row.getInt32(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_long(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_BIGINT:
            {
                int64_t value = row.getInt64(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_long(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_FLOAT:
            {
                double value = row.getDouble(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_double(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_STRING:
            {
                std::string value = row.getString(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_string(return_value, value.c_str(), 0);
                break;
            }
            case voltdb::WIRE_TYPE_TIMESTAMP:
            {
                int64_t value = row.getTimestamp(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                add_next_index_long(return_value, value);
                break;
            }
            case voltdb::WIRE_TYPE_DECIMAL:
            {
                voltdb::Decimal value = row.getDecimal(err, i);
                if (!voltdb::isOk(err)) {
                    return 0;
                }
                // TODO: get float
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

PHP_METHOD(VoltTable, columnIndexFromName)
{
    zval *zobj = getThis();
    volttable_object *obj = (volttable_object *)zend_object_store_get_object(zobj TSRMLS_CC);
    int i = 0;
    std::vector<voltdb::Column> columns = obj->table->columns();
    char *name = NULL;
    int len = 0;

    // Get the name parameter
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (char *)"s", &name, &len) == FAILURE) {
        RETURN_LONG(-1);
    }

    for (i = 0; i < columns.size(); i++) {
        if (columns[i].name().compare(name) == 0) {
            break;
        }
    }

    if (i == columns.size()) {
        // Not found
        RETURN_LONG(-1);
    } else {
        RETURN_LONG(i);
    }
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
    voltdb::errType err = voltdb::errOk;
    voltdb::Row row = obj->it.next(err);
    if (!voltdb::isOk(err)) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL, err TSRMLS_CC);
        RETURN_NULL();
    }

    // Convert the row into a PHP array
    if (array_init(return_value) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), NULL,
                             voltdb::errException TSRMLS_CC);
        RETURN_NULL();
    }

    if (!row_to_array(return_value, row)) {
        RETURN_NULL();
    }
}
