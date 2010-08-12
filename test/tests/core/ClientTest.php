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

    public function testCreateConnectionNoServerRunning() {
        $client = Client::create();
        try {
            $client->createConnection('localhost');
            parent::assertTrue(false, 'Expected ConnectException.');
        } catch (ConnectException $e) {}
    }

    public function testCreateFromPoolNoServerRunning() {
        try {
            Client::createFromPool('localhost');
            parent::assertTrue(false, 'Expected ConnectException.');
        } catch (ConnectException $e) {}
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

}