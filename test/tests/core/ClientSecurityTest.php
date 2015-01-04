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

/**
 * Tests that Client methods involving security behave correctly.
 */
class ClientSecurityTest extends PHPUnit_Framework_TestCase {

    private static $server;

    public static function setUpBeforeClass() {
        self::$server = new SecuritySuite();
        self::$server->start();
    }

    public static function tearDownAfterClass() {
        self::$server->stop();
    }

    public function testCreateConnectionSecurity() {
        $client = Client::create();

        // don't specify credentials
        try {
            $client->createConnection('localhost');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        $config = new ClientConfig('user-1', 'password-x');
        $client = Client::create($config);

        // specify incorrect credentials
        try {
            $client->createConnection('localhost');
            parent::assertTrue(false, 'Shouldn\'t have been able to authenticate.');
        } catch (ConnectException $e) {}

        $config = new ClientConfig('user-1', 'password-1');
        $client = Client::create($config);

        // specify correct credentials
        try {
            $client->createConnection('localhost');
        } catch (ConnectException $e) {
            parent::assertTrue(false, 'Should have been able to authenticate.');
        }
    }

    public function testCreateFromPoolSecurity() {
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
    }

}
