<?php

require_once('../dist/voltdb.php');

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

// Load the database.
$params = $procedure->params();

$params->addString('Hello')->addString('World')->addString('English');
$response = $client->invoke($procedure);
if ($response->failure()) { print($response->toString() . "\n"); }

$params->addString('Bonjour')->addString('Monde')->addString('French');
$response = $client->invoke($procedure);
if ($response->failure()) { print($response->toString() . "\n"); }

$params->addString('Hola')->addString('Mundo')->addString('Spanish');
$response = $client->invoke($procedure);
if ($response->failure()) { print($response->toString() . "\n"); }

$params->addString('Hej')->addString('Verden')->addString('Danish');
$response = $client->invoke($procedure);
if ($response->failure()) { print($response->toString() . "\n"); }

$params->addString('Ciao')->addString('Mondo')->addString('Italian');
$response = $client->invoke($procedure);
if ($response->failure()) { print($response->toString() . "\n"); }

// Describe procedure to retrieve message
$parameters = new Parameters();
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$procedure = new Procedure('Select', $parameters);

// Retrieve the message
$procedure->params()->addString('Spanish');
$response = $client->invoke($procedure);

// Print the response
$results = $response->results();
for ($i = 0; $i < $results->size(); $i++) {
    $table = $results->get($i);
    $iterator = $table->iterator();
    while ($iterator->hasNext()) {
        $row = $iterator->next();
        $hello = $row->getString(0);
        $world = $row->getString(1);
        printf('%s %s' . "\n", $hello, $world);
    }
}
