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

class ClientTest extends PHPUnit_Framework_TestCase {

    /**
     * Client::create(), Client::createFromPool(), and Client::createConnection()
     */

    public function testCreateConnectionNoServerRunning() {
        $client = Client::create();
        parent::setExpectedException('ConnectException');
        $client->createConnection('localhost');
    }

    public function testCreateFromPoolNoServerRunning() {
        parent::setExpectedException('ConnectException');
        Client::createFromPool('localhost');
    }

    public function testCreateConnectionServerRunning() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');

        $server->stop();
    }

    public function testCreateFromPoolServerRunning() {
        $server = new HelloWorld();
        $server->start();

        Client::createFromPool('localhost');

        $server->stop();
    }

    public function testCreateConnectionSecurity() {
        $server = new SecuritySuite();
        $server->start();

        $client = Client::create();

        // don't specify credentials
        try {
            $client->createConnection('localhost');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        // specify incorrect credentials
        try {
            $client->createConnection('localhost', 'user-1', 'password-x');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        // specify correct credentials
        try {
            $client->createConnection('localhost', 'user-1', 'password-1');
        } catch (ConnectException $e) {
            parent::assertTrue(false, 'Should have been able to authenticate.');
        }

        $server->stop();
    }

    public function testCreateFromPoolSecurity() {
        $server = new SecuritySuite();
        $server->start();

        // don't specify credentials
        try {
            Client::createFromPool('localhost');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        // specify incorrect credentials
        try {
            Client::createFromPool('localhost', 'user-1', 'password-x');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        // specify correct credentials
        try {
            Client::createFromPool('localhost', 'user-1', 'password-1');
        } catch (ConnectException $e) {
            parent::assertTrue(false, 'Should have been able to authenticate.');
        }

        $server->stop();
    }

    /**
     * Client::invoke()
     */

    public function testInvokeNoConnection() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->invoke($this->getSelect());
    }

    public function testInvoke() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');
        $this->doInvokeTest($client);

        $server->stop();
    }

    public function testInvokeFromPool() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::createFromPool('localhost');
        $this->doInvokeTest($client);

        $server->stop();
    }

    private function doInvokeTest($client) {
        $this->doInsert($client);
        $response = $client->invoke($this->getSelect());
        $this->verifySelect($response);
    }

    /**
     * Client::invokeAsync()
     */

    public function testInvokeAsyncNoConnection() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->invokeAsync($this->getSelect(), new ClientTestCallback(null));
    }

    public function testInvokeAsync() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');
        $this->doInvokeAsyncTest($client);

        $server->stop();
    }

    public function testInvokeAsyncFromPool() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::createFromPool('localhost');
        $this->doInvokeAsyncTest($client);

        $server->stop();
    }

    private function doInvokeAsyncTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCallback($this);
        $client->invokeAsync($this->getSelect(), $callback);
        while (!$client->drain()) {}
        parent::assertEquals(1, $callback->getInvocationCount());
    }

    /**
     * Client::runOnce()
     */

    public function testRunOnceNoConnections() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->runOnce();
    }

    public function testRunOnce() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');
        $this->doRunOnceTest($client);

        $server->stop();
    }

    public function testRunOnceFromPool() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::createFromPool('localhost');
        $this->doRunOnceTest($client);

        $server->stop();
    }


    // this can go into an infinite loop upon failure
    private function doRunOnceTest($client) {
        $this->doInsert($client);

        $callback = new ClientTestCallback($this);

        // do it 1 time
        $client->invokeAsync($this->getSelect(), $callback);
        parent::assertEquals(0, $callback->getInvocationCount());
        while ($callback->getInvocationCount() !== 1) {
            $client->runOnce();
        }

        // do it 2 times
        $client->invokeAsync($this->getSelect(), $callback);
        $client->invokeAsync($this->getSelect(), $callback);
        while ($callback->getInvocationCount() !== 3) {
            $client->runOnce();
        }
    }

    /**
     * Client::run()
     */

    public function testRunNoConnections() {
        $client = Client::create();
        parent::setExpectedException('NoConnectionsException');
        $client->run();
    }

    public function testRun() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');
        $this->doRunTest($client);

        $server->stop();
    }

    public function testRunFromPool() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::createFromPool('localhost');
        $this->doRunTest($client);

        $server->stop();
    }

    // this can go into an infinite loop upon failure
    private function doRunTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCountingCallback($this, 3);
        $client->invokeAsync($this->getSelect(), $callback);
        $client->invokeAsync($this->getSelect(), $callback);
        $client->invokeAsync($this->getSelect(), $callback);
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

    public function testDrain() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::create();
        $client->createConnection('localhost');
        $this->doDrainTest($client);

        $server->stop();
    }

    public function testDrainFromPool() {
        $server = new HelloWorld();
        $server->start();

        $client = Client::createFromPool('localhost');
        $this->doDrainTest($client);

        $server->stop();
    }

    // this can go into an infinite loop upon failure
    private function doDrainTest($client) {
        $this->doInsert($client);
        $callback = new ClientTestCallback($this);
        $client->invokeAsync($this->getSelect(), $callback);
        $client->invokeAsync($this->getSelect(), $callback);
        $client->invokeAsync($this->getSelect(), $callback);
        $client->drain();
    }

    /**
     * Test helpers
     */

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

}