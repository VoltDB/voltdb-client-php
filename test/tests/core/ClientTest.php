<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2015 VoltDB Inc.
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

class ClientTest extends PHPUnit_Framework_TestCase {

    private static $server;

    public static function setUpBeforeClass() {
        self::$server = new HelloWorld();
        self::$server->start();
    }

    public static function tearDownAfterClass() {
        self::$server->stop();
    }

    /**
     * Client::create(), Client::createFromPool(), and Client::createConnection()
     */

    public function testCreateConnectionServerRunning() {
        $client = Client::create();
        $client->createConnection('localhost');
    }

    public function testCreateFromPoolServerRunning() {
        Client::createFromPool('localhost');
    }

    /**
     * Client::invoke() - sync
     */

    public function testInvokeSync() {
        $client = Client::create();
        $client->createConnection('localhost');
        $this->doInvokeSyncTest($client);
    }

    public function testInvokeSyncFromPool() {
        $client = Client::createFromPool('localhost');
        $this->doInvokeSyncTest($client);
    }

    private function doInvokeSyncTest($client) {
        $this->doInsert($client);
        $response = $client->invoke($this->getSelect());
        $this->verifySelect($response);

        $this->doDelete($client);
    }

    /**
     * Client::invoke() - async
     */

    public function testInvokeAsync() {
        $client = Client::create();
        $client->createConnection('localhost');
        $this->doInvokeAsyncTest($client);
    }

    public function testInvokeAsyncFromPool() {
        $client = Client::createFromPool('localhost');
        $this->doInvokeAsyncTest($client);
    }

    private function doInvokeAsyncTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCallback($this);
        $client->invoke($this->getSelect(), $callback);
        while (!$client->drain()) {}
        parent::assertEquals(1, $callback->getInvocationCount());

        $this->doDelete($client);
    }

    /**
     * Client::runOnce()
     */

    public function testRunOnce() {
        $client = Client::create();
        $client->createConnection('localhost');
        $this->doRunOnceTest($client);
    }

    public function testRunOnceFromPool() {
        $client = Client::createFromPool('localhost');
        $this->doRunOnceTest($client);
    }

    // this can go into an infinite loop upon failure
    private function doRunOnceTest($client) {
        $this->doInsert($client);

        $callback = new ClientTestCallback($this);

        // do it 1 time
        $client->invoke($this->getSelect(), $callback);
        parent::assertEquals(0, $callback->getInvocationCount());
        while ($callback->getInvocationCount() !== 1) {
            $client->runOnce();
        }

        // do it 2 times
        $client->invoke($this->getSelect(), $callback);
        $client->invoke($this->getSelect(), $callback);
        while ($callback->getInvocationCount() !== 3) {
            $client->runOnce();
        }

        $this->doDelete($client);
    }

    /**
     * Client::run()
     */

    public function testRun() {
        $client = Client::create();
        $client->createConnection('localhost');
        $this->doRunTest($client);
    }

    public function testRunFromPool() {
        $client = Client::createFromPool('localhost');
        $this->doRunTest($client);
    }

    // this can go into an infinite loop upon failure
    private function doRunTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCountingCallback($this, 3);
        $client->invoke($this->getSelect(), $callback);
        $client->invoke($this->getSelect(), $callback);
        $client->invoke($this->getSelect(), $callback);
        $client->run();

        $this->doDelete($client);
    }

    /**
     * Client::drain()
     */

    public function testDrain() {
        $client = Client::create();
        $client->createConnection('localhost');
        $this->doDrainTest($client);
    }

    public function testDrainFromPool() {
        $client = Client::createFromPool('localhost');
        $this->doDrainTest($client);
    }

    private function doDrainTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCountingCallback($this, 3);
        $client->invoke($this->getSelect(), $callback);
        $client->invoke($this->getSelect(), $callback);
        $client->invoke($this->getSelect(), $callback);
        $client->drain();
        parent::assertEquals(0, $callback->getInvocationCount());

        $this->doDelete($client);
    }

    /**
     * Test helpers
     */

    private function doDelete($client) {
        $parameters = new Parameters();
        $deleteProcedure = new Procedure('Delete', $parameters);
        $response = $client->invoke($deleteProcedure);

        parent::assertTrue($response->success());
        parent::assertEquals('', $response->statusString());
        parent::assertEquals(-128, $response->appStatusCode());
        parent::assertEquals('', $response->appStatusString());
        parent::assertEquals(1, $response->results()->size());
    }

    private function doInsert($client) {
        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $insertProcedure = new Procedure('Insert', $parameters);
        $insertProcedure->params()->addString('Hola')->addString('Mundo')->addString('Spanish');
        $response = $client->invoke($insertProcedure);

        parent::assertTrue($response->success());
        parent::assertEquals('', $response->statusString());
        parent::assertEquals(-128, $response->appStatusCode());
        parent::assertEquals('', $response->appStatusString());
        parent::assertEquals(1, $response->results()->size());
    }

    private function getSelect() {
        $parameters = new Parameters();
        $parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
        $selectProcedure = new Procedure('Select', $parameters);
        $selectProcedure->params()->addString('Spanish');
        return $selectProcedure;
    }

    public function verifySelect($response) {
        parent::assertTrue($response->success());
        parent::assertEquals('', $response->statusString());
        parent::assertEquals(-128, $response->appStatusCode());
        parent::assertEquals('', $response->appStatusString());
        parent::assertEquals(1, $response->results()->size());

        $results = $response->results();
        $table = $results->get(0);
        parent::assertEquals(1, $table->rowCount());
        $iterator = $table->iterator();
        $row = $iterator->next();
        parent::assertFalse($iterator->hasNext());
        $hello = $row->getString(0);
        $world = $row->getString(1);
        parent::assertEquals('Hola', $hello);
        parent::assertEquals('Mundo', $world);
    }

}

class ClientTestCallback extends ProcedureCallback {

    private $test;
    private $invocationCount = 0;

    public function __construct($test) {
        $this->test = $test;
    }

    public function callback($response) {
        $this->test->verifySelect($response);
        $this->invocationCount++;
        return true;
    }

    public function getInvocationCount() {
        return $this->invocationCount;
    }

}

class ClientTestCountingCallback extends ProcedureCallback {

    private $test;
    private $invocationCount;

    public function __construct($test, $invocationCount) {
        $this->test = $test;
        $this->invocationCount = $invocationCount;
    }

    public function callback($response) {
        $this->test->verifySelect($response);
        return --$this->invocationCount === 0;
    }

    public function getInvocationCount() {
        return $this->invocationCount;
    }

}
