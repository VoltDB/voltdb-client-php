<?php

require('voltdb.php');

// EDIT 'localhost' below to specify one hostname
// running the VoltDB voter sample application.
$voltHostname = 'localhost';

$maxContestants = 12;
$contestantNames =
    'Edwina Burnam,'.
    'Tabatha Gehling,'.
    'Kelly Clauss,'.
    'Jessie Alloway,'.
    'Alana Bregman,'.
    'Jessie Eichman,'.
    'Allie Rogalski,'.
    'Nita Coster,'.
    'Kurt Walser,'.
    'Ericka Dieter,'.
    'Loraine Nygren,'.
    'Tania Mattioli';

$voltClient = Client::create();

try {
    $voltClient->createConnection($voltHostname);
} catch (ConnectException $e) {
    print($e->getMessage() . "\n");
    exit(1);
}

$parameters = new Parameters();
$parameters->push(new Parameter(voltdb::WIRE_TYPE_INTEGER));
$parameters->push(new Parameter(voltdb::WIRE_TYPE_STRING));
$procedure = new Procedure('Initialize', $parameters);
$procedure->params()->addInt32($maxContestants)->addString($contestantNames);
$response = $voltClient->invoke($procedure);
$results = $response->results();

if ($response->failure()) {
   print($response->toString() . "\n");
} else {
   printf('Initialized' . "\n");
}
