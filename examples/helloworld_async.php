<?php

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
$client->invoke($procedure, $callback);

$params->addString('Bonjour')->addString('Monde')->addString('French');
$client->invoke($procedure, $callback);

$params->addString('Hola')->addString('Mundo')->addString('Spanish');
$client->invoke($procedure, $callback);

$params->addString('Hej')->addString('Verden')->addString('Danish');
$client->invoke($procedure, $callback);

$params->addString('Ciao')->addString('Mondo')->addString('Italian');
$client->invoke($procedure, $callback);

// Run the client event loop to poll the network and invoke callbacks.
// The event loop will break on an error or when a callback returns true
$client->run();

// Describe procedure to retrieve message
$parameters = new Parameters();
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$procedure = new Procedure('Select', $parameters);

// Retrieve the message
$procedure->params()->addString('Spanish');
$client->invoke($procedure, new PrintingCallback());

$client->run();
