<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2013 VoltDB Inc.
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

class TypeSuite extends ProjectBuilder {

    public function __construct() {
        parent::__construct();

        $schemas = array();
        $schemas[] = '
            CREATE TABLE types (
                column0_pkey        BIGINT NOT NULL,
                column1_decimal     DECIMAL,
                column2_timestamp   TIMESTAMP,
                column3_bigint      BIGINT,
                column4_integer     INTEGER,
                column5_smallint    SMALLINT,
                column6_tinyint     TINYINT,
                column7_float       FLOAT,
                column8_varchar     VARCHAR(100),
                PRIMARY KEY (column0_pkey)
            );
        ';
        parent::setSchemas($schemas);

        $procedures = array();
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Decimal',
            'INSERT INTO types (column0_pkey, column1_decimal) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Timestamp',
            'INSERT INTO types (column0_pkey, column2_timestamp) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Bigint',
            'INSERT INTO types (column0_pkey, column3_bigint) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Integer',
            'INSERT INTO types (column0_pkey, column4_integer) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Smallint',
            'INSERT INTO types (column0_pkey, column5_smallint) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Tinyint',
            'INSERT INTO types (column0_pkey, column6_tinyint) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Float',
            'INSERT INTO types (column0_pkey, column7_float) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert_Varchar',
            'INSERT INTO types (column0_pkey, column8_varchar) VALUES (?, ?);',
            'types.column0_pkey: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Select',
            'SELECT * FROM types WHERE column0_pkey = ?;',
            'types.column0_pkey: 0');
        parent::setProcedures($procedures);

        $partitions = array();
        $partitions[] = new PartitionInfo('types', 'column0_pkey');
        parent::setPartitions($partitions);
    }

}
