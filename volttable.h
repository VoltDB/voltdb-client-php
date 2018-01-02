/* This file is part of VoltDB.
 * Copyright (C) 2008-2018 VoltDB Inc.
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

#ifndef VOLT_TABLE_H
#define VOLT_TABLE_H

#include "Table.h"
#include "TableIterator.h"

// VoltTable
PHP_METHOD(VoltTable, statusCode);
PHP_METHOD(VoltTable, rowCount);
PHP_METHOD(VoltTable, columnCount);
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
