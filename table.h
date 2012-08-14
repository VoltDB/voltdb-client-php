#ifndef VOLT_TABLE_H
#define VOLT_TABLE_H

#include "Table.h"
#include "TableIterator.h"

// VoltTable
PHP_METHOD(VoltTable, statusCode);
PHP_METHOD(VoltTable, rowCount);
PHP_METHOD(VoltTable, columnCount);
PHP_METHOD(VoltTable, columnIndexFromName);
PHP_METHOD(VoltTable, hasMoreRows);
PHP_METHOD(VoltTable, nextRow);

struct volttable_object {
    zend_object std;
    voltdb::Table *table;
    voltdb::TableIterator it;
};

void create_volttable_class(void);
struct volttable_object *instantiate_volttable(zval *return_val, voltdb::Table &table);

#endif  // VOLT_TABLE_H
