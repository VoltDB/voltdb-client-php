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
 * Tests that trying to add a wrong parameter type throws an exception for each possible combination of given/expected.
 */
class ParameterTest extends PHPUnit_Framework_TestCase {

    /**
     * This section prepares the tests.
     */

    public function testDecimal() {
        $this->runTests(voltdb::WIRE_TYPE_DECIMAL);
    }

    public function testTimestamp() {
        $this->runTests(voltdb::WIRE_TYPE_TIMESTAMP);
    }

    public function testBigint() {
        $this->runTests(voltdb::WIRE_TYPE_BIGINT);
    }

    public function testInteger() {
        $this->runTests(voltdb::WIRE_TYPE_INTEGER);
    }

    public function testSmallint() {
        $this->runTests(voltdb::WIRE_TYPE_SMALLINT);
    }

    public function testTinyint() {
        $this->runTests(voltdb::WIRE_TYPE_TINYINT);
    }

    public function testFloat() {
        $this->runTests(voltdb::WIRE_TYPE_FLOAT);
    }

    public function testVarchar() {
        $this->runTests(voltdb::WIRE_TYPE_STRING);
    }

    /**
     * This section does the actual testing.
     */

    private function runTests($type) {
        $parameters = new Parameters();
        $parameters->push(new Parameter($type));
        $procedure = new Procedure('', $parameters);
        $params = $procedure->params();

        $this->doTest($params, 'addDecimal', SampleValues::$DECIMAL, $type !== voltdb::WIRE_TYPE_DECIMAL);
        $this->doTest($params, 'addTimestamp', SampleValues::$TIMESTAMP, $type !== voltdb::WIRE_TYPE_TIMESTAMP);
        $this->doTest($params, 'addInt64', SampleValues::$BIGINT, $type !== voltdb::WIRE_TYPE_BIGINT);
        $this->doTest($params, 'addInt32', SampleValues::$INTEGER, $type !== voltdb::WIRE_TYPE_INTEGER);
        $this->doTest($params, 'addInt16', SampleValues::$SMALLINT, $type !== voltdb::WIRE_TYPE_SMALLINT);
        $this->doTest($params, 'addInt8', SampleValues::$TINYINT, $type !== voltdb::WIRE_TYPE_TINYINT);
        $this->doTest($params, 'addDouble', SampleValues::$FLOAT, $type !== voltdb::WIRE_TYPE_FLOAT);
        $this->doTest($params, 'addString', SampleValues::$VARCHAR, $type !== voltdb::WIRE_TYPE_STRING);
    }

    private function doTest($params, $method, $value, $expectException) {
        try {
            $params->$method($value);
            if ($expectException) {
                parent::assertTrue(false, 'Expected ParamMismatchException.');
            } else {
                parent::assertTrue(true);
            }
        } catch (ParamMismatchException $e) {
            if (!$expectException) {
                parent::assertTrue(false, 'Didn\'t expect ParamMismatchException.');
            } else {
                parent::assertTrue(true);
            }
        }
    }

}
