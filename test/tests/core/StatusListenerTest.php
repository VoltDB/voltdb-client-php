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

class StatusListenerTest extends PHPUnit_Framework_TestCase {

    /**
     * StatusListener::uncaughtException()
     */

    public function testUncaughtException() {
        $server = new HelloWorld();
        $server->start();

        $callback = new StatusListenerTestCallback();
        $listener = new StatusListenerTestListener($this, $callback);
        $client = Client::create($listener);
        $client->createConnection('localhost');

        $this->doUncaughtExceptionTest($client, $callback, $listener);

        $server->stop();
    }

    public function testUncaughtExceptionFromPool() {
        $server = new HelloWorld();
        $server->start();

        $callback = new StatusListenerTestCallback();
        $listener = new StatusListenerTestListener($this, $callback);
        $client = Client::createFromPool('localhost', '', '', $listener);

        $this->doUncaughtExceptionTest($client, $callback, $listener);

        $server->stop();
    }

    private function doUncaughtExceptionTest($client, $callback, $listener) {

        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $insertProcedure = new Procedure('Insert', $parameters);
        $insertProcedure->params()->addString('Hola')->addString('Mundo')->addString('Spanish');
        $client->invoke($insertProcedure, $callback);

        $client->run();
        parent::assertTrue($listener->called('uncaughtException'));
    }

    /**
     * StatusListener::connectionLost()
     */

    public function testConnectionLost() {
        $server = new HelloWorld();
        $server->start();

        $listener = new StatusListenerTestListener($this);
        $client = Client::create($listener);
        $client->createConnection('localhost');

        $this->doConnectionLostTest($client);

        $server->stop();

        $client->run();
        parent::assertTrue($listener->called('connectionLost'));
    }

    public function testConnectionLostFromPool() {
        $server = new HelloWorld();
        $server->start();

        $listener = new StatusListenerTestListener($this);
        $client = Client::createFromPool('localhost', '', '', $listener);

        $this->doConnectionLostTest($client);

        $server->stop();

        $client->run();
        parent::assertTrue($listener->called('connectionLost'));
    }

    public function doConnectionLostTest($client) {
        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $insertProcedure = new Procedure('Insert', $parameters);
        $insertProcedure->params()->addString('Hola')->addString('Mundo')->addString('Spanish');
        $client->invoke($insertProcedure);
    }

    /**
     * StatusListener::backpressure()
     */

    public function testBackpressure() {
        $server = new HelloWorld();
        $server->start();

        $listener = new StatusListenerTestListener($this);
        $client = Client::create($listener);
        $client->createConnection('localhost');

        $this->doBackpressureTest($client, $listener);

        $server->stop();
    }

    public function testBackpressureFromPool() {
        $server = new HelloWorld();
        $server->start();

        $listener = new StatusListenerTestListener($this);
        $client = Client::createFromPool('localhost', '', '', $listener);

        $this->doBackpressureTest($client, $listener);

        $server->stop();
    }

    public function doBackpressureTest($client, $listener) {
        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $insertProcedure = new Procedure('Insert', $parameters);

        // this is tuned to trigger the 256k backpressure threshold
        $longString = '';
        for ($i = 0; $i < 256 * 1024 / 3; $i++) {
            $longString .= ' ';
        }
        $insertProcedure->params()->addString($longString)->addString($longString)->addString($longString);
        $client->invoke($insertProcedure, new StatusListenerTestCallback());
        parent::assertFalse($listener->called('backpressure'));

        $insertProcedure->params()->addString('Hola')->addString('Mundo')->addString('Spanish');
        $client->invoke($insertProcedure, new StatusListenerTestCallback());
        parent::assertTrue($listener->called('backpressure'));
    }

}

class StatusListenerTestListener extends StatusListener {

    private $test;
    private $callback;

    private $uncaughtException = false;
    private $connectionLost = false;
    private $backpressure = false;

    public function __construct($test, $callback = null) {
        $this->test = $test;
        $this->callback = $callback;
    }

    public function uncaughtException($exception, $callback) {
        $this->test->assertEquals('StatusListenerTestException', get_class($exception));
        $this->test->assertEquals('Throwing a callback exception.', $exception->getMessage());
        $this->test->assertEquals($this->callback, $callback);
        $this->uncaughtException = true;
        return true;
    }

    public function connectionLost($hostname, $connectionsLeft) {
        $this->connectionLost = true;
        $this->test->assertEquals('localhost', $hostname);
        $this->test->assertEquals(0, $connectionsLeft);
        return true;
    }

    public function backpressure($hasBackpressure) {
        $this->test->assertTrue($hasBackpressure);
        $this->backpressure = true;
        return true;
    }

    public function called($method) {
        return $this->$method;
    }

}

class StatusListenerTestCallback extends ProcedureCallback {

    public function callback($response) {
        throw new StatusListenerTestException('Throwing a callback exception.');
    }

}

class StatusListenerTestException extends Exception {}
