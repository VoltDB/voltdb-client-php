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

require('../dist/voltdb.php');

class CountingCallback extends ProcedureCallback {

    private $count;

    public function __construct($count) {
        $this->count = $count;
    }

    public function callback($response) {
        $this->count--;

        if ($response->failure()) {
            print($response->toString() . "\n");
        }

        return $this->count === 0;
    }

}

class PrintingCallBack extends ProcedureCallback {

    public function callback($response) {
        $results = $response->results();
        for ($i = 0; $i < $results->size(); $i++) {
            $table = $results->get($i);
            $iterator = $table->iterator();
            while ($iterator->hasNext()) {
                $row = $iterator->next();
                $hello = $row->getString('HELLO');
                $world = $row->getString('WORLD');
                printf('%s %s' . "\n", $hello, $world);
            }
        }

        return true;
    }

}

// Instantiate a client and connect to the database.
$client = Client::create();
try {
    $client->createConnection('localhost');
} catch (ConnectException $e) {
    print($e->getMessage() . "\n");
    exit(1);
}

// Describe the stored procedure to be invoked
$parameters = new Parameters();
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$procedure = new Procedure('Insert', $parameters);
$callback = new CountingCallback(5);

// Load the database.
$params = $procedure->params();

$params->addString('Hello')->addString('World')->addString('English');
$client->invokeAsync($procedure, $callback);

$params->addString('Bonjour')->addString('Monde')->addString('French');
$client->invokeAsync($procedure, $callback);

$params->addString('Hola')->addString('Mundo')->addString('Spanish');
$client->invokeAsync($procedure, $callback);

$params->addString('Hej')->addString('Verden')->addString('Danish');
$client->invokeAsync($procedure, $callback);

$params->addString('Ciao')->addString('Mondo')->addString('Italian');
$client->invokeAsync($procedure, $callback);

// Run the client event loop to poll the network and invoke callbacks.
// The event loop will break on an error or when a callback returns true
$client->run();

// Describe procedure to retrieve message
$parameters = new Parameters();
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$procedure = new Procedure('Select', $parameters);

// Retrieve the message
$procedure->params()->addString('Spanish');
$client->invokeAsync($procedure, new PrintingCallback());

$client->run();