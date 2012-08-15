<?php

// EDIT 'localhost' below to specify one or two
// hostnames running the VoltDB voter sample application.
$host1 = 'localhost';
$host2 = 'localhost';

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
    $voltClient = new VoltClient();
    $voltClient->connect($hostname, '', '');
} catch (Exception $e) {
    print($e->getMessage() . "\n");
    exit(1);
}

$goodVotes = 0;
$badVotes = 0;

// vote 5 times
for ($i = 0; $i < 5; $i++) {
    $phoneNumber = mt_rand(0, 9999999999);
    $contestantNumber = mt_rand(0, $maxContestants) * mt_rand(0, $maxContestants) % $maxContestants + 1;
    try {
        $resp = $voltClient->invoke("Vote", array(strval($phoneNumber), strval($contestantNumber), strval($maxVotesPerPhoneNumber)));
        if ($resp->statusCode() == VoltInvocationResponse::SUCCESS) {
            $goodVotes++;
        }
    } catch (Exception $e) {
        $badVotes++;
        echo 'Exception '. $e->getCode() . '\n';
        continue;
    }
}

printf("<html><body>Accepted votes:" . $goodVotes .
       " Failed votes:" . $badVotes . "</body></html>\n");

