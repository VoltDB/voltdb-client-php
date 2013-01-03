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

class HelloWorld extends ProjectBuilder {

    public function __construct() {
        parent::__construct();

        $schemas = array();
        $schemas[] = '
            CREATE TABLE HELLOWORLD (
                HELLO VARCHAR(15),
                WORLD VARCHAR(15),
                DIALECT VARCHAR(15) NOT NULL,
                PRIMARY KEY (DIALECT)
            );
        ';
        parent::setSchemas($schemas);

        $procedures = array();
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert',
            'INSERT INTO HELLOWORLD VALUES (?, ?, ?);',
            'HELLOWORLD.DIALECT: 2');
        $procedures[] = new ProcedureInfo(
            array(),
            'Select',
            'SELECT HELLO, WORLD FROM HELLOWORLD WHERE DIALECT = ?;',
            'HELLOWORLD.DIALECT: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Delete',
            'DELETE FROM HELLOWORLD;');
        parent::setProcedures($procedures);

        $partitions = array();
        $partitions[] = new PartitionInfo('HELLOWORLD', 'DIALECT');
        parent::setPartitions($partitions);
    }

}
