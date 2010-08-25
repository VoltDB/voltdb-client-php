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
 * Tests that all Client methods behave correctly and throw proper exceptions when no VoltDB server is running.
 */
class ClientOfflineTest extends PHPUnit_Framework_TestCase {

    /**
     * Client::create(), Client::createConnection()
     */

    public function testCreateConnectionNoServerRunning() {
        $client = Client::create();
        parent::setExpectedException('ConnectException');
        $client->createConnection('localhost');
    }

    /**
     * Client::createFromPool()
     */

    public function testCreateFromPoolNoServerRunning() {
        parent::setExpectedException('ConnectException');
        Client::createFromPool('localhost');
    }

    /**
     * Client::invoke() - sync
     */

    public function testInvokeSyncNoConnection() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->invoke($this->getSelect());
    }

    /**
     * Client::invoke() - async
     */

    public function testInvokeAsyncNoConnection() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->invoke($this->getSelect(), new ClientOfflineTestCallback());
    }

    /**
     * Client::runOnce()
     */

    public function testRunOnceNoConnections() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->runOnce();
    }

    /**
     * Client::run()
     */

    public function testRunNoConnections() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->run();
    }

    /**
     * Client::drain()
     */

    public function testDrainNoConnections() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->drain();
    }

    /**
     * Test helper
     */

    private function getSelect() {
        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $selectProcedure = new Procedure('Select', $parameters);
        $selectProcedure->params()->addString('Spanish');
        return $selectProcedure;
    }

}

class ClientOfflineTestCallback extends ProcedureCallback {

    public function callback($response) {}

}
