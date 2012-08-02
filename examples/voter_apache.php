<?php

require('voltdb.php');

// EDIT 'localhost' below to specify one or two
// hostnames running the VoltDB voter sample application.
$host1 = 'localhost';
$host2 = 'localhost';

class AsyncCallback extends \volt\ProcedureCallback {
    public static $goodVotes = 0;
    public static $badVotes = 0;
    function callback($response) {
        if ($response->failure()) {
            AsyncCallback::$badVotes++;
        } else {
            AsyncCallback::$goodVotes++;
        }
    }
}

// php not-reentrant. callback is reused.
$callback = new AsyncCallback();

// some voter configuration.
$maxVotesPerPhoneNumber = 5;
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

// Distribute intitiation across both nodes.
$client = mt_rand(0,100);
if ($client < 50) {
    $hostname = $host1;
} else {
    $hostname = $host2;
}

// Use a client that has a cached connection from the pool.
try {
    $voltClient = \volt\Client::createFromPool($hostname, '', '', null, 21212);
} catch (Exception $e) {
    print($e->getMessage() . "\n");
    exit(1);
}

// declare the vote procedure
$parameters = new \volt\Parameters();
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_BIGINT));
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_TINYINT));
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_BIGINT));
$procedure = new \volt\Procedure('Vote', $parameters);

// vote 5 times
for ($i = 0; $i < 5; $i++) {
    $phoneNumber = mt_rand(0, 9999999999);
    $contestantNumber = mt_rand(0, $maxContestants) * mt_rand(0, $maxContestants) % $maxContestants + 1;
    $procedure->params()->addInt64($phoneNumber)->addInt8($contestantNumber)->addInt64($maxVotesPerPhoneNumber);
    $voltClient->invoke($procedure, $callback);
}

// collect responses
while (!$voltClient->drain()) {
}

printf("<html><body>Accepted votes:" . AsyncCallback::$goodVotes .
       " Failed votes:" . AsyncCallback::$badVotes . "</body></html>");

