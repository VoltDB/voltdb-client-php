<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2010 VoltDB L.L.C.
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

/**
 * Tests that parameters sent to VoltDB come back as the correct type with the correct data.
 */
class WireTypeTest extends PHPUnit_Framework_TestCase {

    private static $server;
    private static $client;

    public static function setUpBeforeClass() {
        self::$server = new TypeSuite();
        self::$server->start();

        self::$client = Client::create();
        self::$client->createConnection('localhost');
    }

    public static function tearDownAfterClass() {
        self::$server->stop();
    }

    public function testDecimal() {
        $key = 1;
        $value = SampleValues::$DECIMAL;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_DECIMAL));
        $procedure = new Procedure('Insert_Decimal', $parameters);
        $procedure->params()->addInt64($key)->addDecimal($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value->toString(), $row->getDecimal('column1_decimal')->toString());
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_DECIMAL, $columns->get(1)->type());
    }

    public function testTimestamp() {
        $key = 2;
        $value = SampleValues::$TIMESTAMP;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_TIMESTAMP));
        $procedure = new Procedure('Insert_Timestamp', $parameters);
        $procedure->params()->addInt64($key)->addTimestamp($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getTimestamp('column2_timestamp'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_TIMESTAMP, $columns->get(2)->type());
    }

    public function testBigint() {
        $key = 3;
        $value = SampleValues::$BIGINT;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Insert_Bigint', $parameters);
        $procedure->params()->addInt64($key)->addInt64($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getInt64('column3_bigint'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(3)->type());
    }

    public function testInteger() {
        $key = 4;
        $value = SampleValues::$INTEGER;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_INTEGER));
        $procedure = new Procedure('Insert_Integer', $parameters);
        $procedure->params()->addInt64($key)->addInt32($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getInt32('column4_integer'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_INTEGER, $columns->get(4)->type());
    }

    public function testSmallint() {
        $key = 5;
        $value = SampleValues::$SMALLINT;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_SMALLINT));
        $procedure = new Procedure('Insert_Smallint', $parameters);
        $procedure->params()->addInt64($key)->addInt16($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getInt16('column5_smallint'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_SMALLINT, $columns->get(5)->type());
    }

    public function testTinyint() {
        $key = 6;
        $value = SampleValues::$TINYINT;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_TINYINT));
        $procedure = new Procedure('Insert_Tinyint', $parameters);
        $procedure->params()->addInt64($key)->addInt8($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getInt8('column6_tinyint'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_TINYINT, $columns->get(6)->type());
    }

    public function testFloat() {
        $key = 7;
        $value = SampleValues::$FLOAT;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_FLOAT));
        $procedure = new Procedure('Insert_Float', $parameters);
        $procedure->params()->addInt64($key)->addDouble($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getDouble('column7_float'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_FLOAT, $columns->get(7)->type());
    }

    public function testVarchar() {
        $key = 8;
        $value = SampleValues::$VARCHAR;

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $procedure = new Procedure('Insert_Varchar', $parameters);
        $procedure->params()->addInt64($key)->addString($value);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_BIGINT));
        $procedure = new Procedure('Select', $parameters);
        $procedure->params()->addInt64($key);
        $response = self::$client->invoke($procedure);
        parent::assertTrue($response->success());
        $results = $response->results();
        $row = $results->get(0)->iterator()->next();
        parent::assertEquals($key, $row->getInt64('column0_pkey'));
        parent::assertEquals($value, $row->getString('column8_varchar'));
        $columns = $row->columns();
        parent::assertEquals(voltdb::WIRE_TYPE_BIGINT, $columns->get(0)->type());
        parent::assertEquals(voltdb::WIRE_TYPE_STRING, $columns->get(8)->type());
    }

}
