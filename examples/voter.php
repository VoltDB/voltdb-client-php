<?php

require('../dist/voltdb.php');

// this class is a shared storage area for AsyncCallback and the other code
class Stats {

    public static $minExecutionMilliseconds = 999999999;
    public static $maxExecutionMilliseconds = -1;
    public static $totExecutionMilliseconds = 0;
    public static $totExecutions = 0;
    public static $totExecutionsLatency = 0;
    public static $latencyCounter = array(0, 0, 0, 0, 0, 0, 0, 0, 0);

    public static $voteResultCounter = array(0, 0, 0);

    public static $checkLatency = false;

    public static $numSpCalls = 0;

    public static $minAllowedOutstanding;
    public static $maxAllowedOutstanding;

}

class AsyncCallback extends \volt\ProcedureCallback {

    function callback($response) {
        if ($response->failure()) {
            printf('Failed to execute' . "\n");
            printf($response->toString() . "\n");
            exit(1);
        }

        Stats::$totExecutions++;
        $results = $response->results();
        $table = $results->get(0);
        $iterator = $table->iterator();
        $row = $iterator->next();
        $voteResult = $row->getInt64(0);
        Stats::$voteResultCounter[$voteResult]++;

        if (Stats::$checkLatency) {
            $executionTime = $response->clusterRoundTripTime();
            Stats::$totExecutionsLatency++;
            Stats::$totExecutionMilliseconds += $executionTime;
            if ($executionTime < Stats::$minExecutionMilliseconds) {
                Stats::$minExecutionMilliseconds = $executionTime;
            }
            if ($executionTime > Stats::$maxExecutionMilliseconds) {
                Stats::$maxExecutionMilliseconds = $executionTime;
            }

            $latencyBucket = $executionTime / 25;
            if ($latencyBucket > 8) {
                $latencyBucket = 8;
            }
            Stats::$latencyCounter[$latencyBucket]++;
        }

        return Stats::$numSpCalls - Stats::$totExecutions < Stats::$minAllowedOutstanding;
    }

}

if ($argc !== 8 + 1) { // 8 arguments and the name of the script
    print('usage: php voter.php [votes per phone number] [transactions per second] ' .
        '[minimum outstanding] [maximum outstanding] [client feedback interval (seconds)] [test duration (seconds)] ' .
        '[lag record delay (seconds)] [server list (comma separated)]' . "\n");
    exit(1);
}


$maxVotesPerPhoneNumber = (int) $argv[1];
$transactionsPerSecond = $argv[2];
$transactionsPerMilli = $transactionsPerSecond / 1000;
Stats::$minAllowedOutstanding = $argv[3];
Stats::$maxAllowedOutstanding = $argv[4];
$clientFeedbackIntervalSecs = $argv[5];
$testDurationSecs = $argv[6];
$lagLatencySeconds = $argv[7];
$serverList = $argv[8];
$lagLatencyMillis = $lagLatencySeconds * 1000;
$thisOutstanding = 0;
$lastOutstanding = 0;

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

$maxContestants = 12;

printf('Allowing %d votes per phone number' . "\n", $maxVotesPerPhoneNumber);
printf('Allowing between %s and %s oustanding SP calls at a time' . "\n",
    number_format(Stats::$minAllowedOutstanding), number_format(Stats::$maxAllowedOutstanding));
printf('Submitting %s SP Calls/sec' . "\n", number_format($transactionsPerSecond));
printf('Feedback interval = %s second(s)' . "\n", number_format($clientFeedbackIntervalSecs));
printf('Running for %s second(s)' . "\n", number_format($testDurationSecs));
printf('Latency not recorded for %s second(s)' . "\n", $lagLatencySeconds);

$phoneNumber;
$contestantNumber;

$transactionsThisSecond = 0;
$lastMillisecond = microtime(true) * 1000;
$thisMillisecond = microtime(true) * 1000;

$voltClient = \volt\Client::create();

$voltServers = explode(',', $serverList);
foreach ($voltServers as $thisServer) {
    $thisServer = trim($thisServer);
    printf('Connecting to server: \'%s\'' . "\n", $thisServer);
    try {
        $voltClient->createConnection($thisServer);
    } catch (Exception $e) {
        print($e->getMessage() . "\n");
        exit(1);
    }
}

$parameters = new \volt\Parameters();
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_INTEGER));
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_STRING));
$procedure = new \volt\Procedure('Initialize', $parameters);
$procedure->params()->addInt32($maxContestants)->addString($contestantNames);
$response = $voltClient->invoke($procedure);
$results = $response->results();
$table = $results->get(0);
$iterator = $table->iterator();
$row = $iterator->next();
printf('Running for %d contestant(s)' . "\n", $maxContestants);

$startTime = microtime(true) * 1000;
$endTime = $startTime + 1000 * $testDurationSecs;
$currentTime = $startTime;
$lastFeedbackTime = $startTime;
$startRecordingLatency = $startTime + $lagLatencyMillis;

$callback = new AsyncCallback();

$parameters = new \volt\Parameters();
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_BIGINT));
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_TINYINT));
$parameters->push(new \volt\Parameter(\volt\voltdb::WIRE_TYPE_BIGINT));
$procedure = new \volt\Procedure('Vote', $parameters);

while ($endTime > $currentTime) {
    Stats::$numSpCalls++;
    if (Stats::$numSpCalls - Stats::$totExecutions > Stats::$maxAllowedOutstanding) {
        $voltClient->run();
    }

    $phoneNumber = mt_rand(0, 9999999999);
    $contestantNumber = mt_rand(0, $maxContestants) * mt_rand(0, $maxContestants) % $maxContestants + 1;
    if (Stats::$numSpCalls % 100 === 0) {
        // introduce a potentially bad contestant number every 100 votes
        $contestantNumber = (mt_rand(0, $maxContestants) + 1) * 2;
    }

    $procedure->params()->addInt64($phoneNumber)->addInt8($contestantNumber)->addInt64($maxVotesPerPhoneNumber);
    $voltClient->invoke($procedure, $callback);

    $transactionsThisSecond++;
    if ($transactionsThisSecond >= $transactionsPerMilli) {
        $voltClient->runOnce();
        $thisMillisecond = microtime(true) * 1000;
        while ($thisMillisecond <= $lastMillisecond) {
            $thisMillisecond = microtime(true) * 1000;
        }
        $lastMillisecond = $thisMillisecond;
        $transactionsThisSecond = 0;
    }

    $currentTime = microtime(true) * 1000;

    if (!Stats::$checkLatency && $currentTime >= $startRecordingLatency) {
        Stats::$checkLatency = true;
    }

    if ($currentTime >= $lastFeedbackTime + $clientFeedbackIntervalSecs * 1000)
    {
        $elapsedTimeMillis2 = microtime(true) * 1000 - $startTime;
        $lastFeedbackTime = $currentTime;
        $runTimeMillis = $endTime - $startTime;
        $elapsedTimeSec2 = $elapsedTimeMillis2 / 1000;

        if (Stats::$totExecutionsLatency === 0) {
            $totExecutionsLatency = 1;
        }

        $percentComplete = $elapsedTimeMillis2 / $runTimeMillis * 100;
        if ($percentComplete > 100.0) {
            $percentComplete = 100.0;
        }

        $thisOutstanding = Stats::$numSpCalls - Stats::$totExecutions;
        $avgLatency = 0; //Stats::$totExecutionMilliseconds / Stats::$totExecutionsLatency;

        printf('%.3f%% Complete | SP Calls: %s at %s SP/sec | outstanding = %d (%d) | min = %d | max = %d | avg = %.2f' . "\n",
            $percentComplete,
            number_format(Stats::$numSpCalls),
            number_format(Stats::$numSpCalls / $elapsedTimeSec2, 2),
            $thisOutstanding,
            $thisOutstanding - $lastOutstanding,
            Stats::$minExecutionMilliseconds,
            Stats::$maxExecutionMilliseconds,
            $avgLatency
        );

        $lastOutstanding = $thisOutstanding;
    }
}

while (!$voltClient->drain()) {}

$elapsedTimeMillis = microtime(true) * 1000 - $startTime;
$elapsedTimeSec = $elapsedTimeMillis / 1000;

print("\n");
print("\n");
print('*************************************************************************' . "\n");
print('Voting Results' . "\n");
print('*************************************************************************' . "\n");
printf(' - Accepted votes = %s' . "\n", number_format(Stats::$voteResultCounter[0]));
printf(' - Rejected votes (invalid contestant) = %s' . "\n", number_format(Stats::$voteResultCounter[1]));
printf(' - Rejected votes (voter over limit) = %s' . "\n", number_format(Stats::$voteResultCounter[2]));
print("\n");

$winnerName = "<<UNKNOWN>>";

$winnerVotes = -1;
$parameters = new \volt\Parameters();
$procedure = new \volt\Procedure('Results', $parameters);
$response = $voltClient->invoke($procedure);
$results = $response->results();
$table = $results->get(0);
$rowCount = $table->rowCount();
if ($rowCount === 0) {
    print(' - No results to report.');
} else {
    $iterator = $table->iterator();
    for ($i = 0; $i < $rowCount; $i++) {
        $row = $iterator->next();
        $resultName = $row->getString(0);
        $resultVotes = $row->getInt64(1);
        printf(' - Contestant %s received %s vote(s)' . "\n", $resultName, $resultVotes);
        if ($resultVotes > $winnerVotes) {
            $winnerVotes = $resultVotes;
            $winnerName = $resultName;
        }
    }
}

print("\n");
printf(' - Contestant %s was the winner with %s vote(s)' . "\n", $winnerName, $winnerVotes);
print("\n");
print('*************************************************************************' . "\n");
print('System Statistics' . "\n");
print('*************************************************************************' . "\n");
printf(' - Ran for %s seconds' . "\n", number_format($elapsedTimeSec, 2));
printf(' - Performed %s Stored Procedure calls' . "\n", number_format(Stats::$numSpCalls));
printf(' - At %s calls per second' . "\n", number_format(Stats::$numSpCalls / $elapsedTimeSec, 2));
printf(' - Average Latency = %s ms' . "\n", number_format(Stats::$totExecutionMilliseconds / Stats::$totExecutionsLatency, 2));
printf(' -   Latecy   0ms -  25ms = %s' . "\n", number_format(Stats::$latencyCounter[0]));
printf(' -   Latecy  25ms -  50ms = %s' . "\n", number_format(Stats::$latencyCounter[1]));
printf(' -   Latecy  50ms -  75ms = %s' . "\n", number_format(Stats::$latencyCounter[2]));
printf(' -   Latecy  75ms - 100ms = %s' . "\n", number_format(Stats::$latencyCounter[3]));
printf(' -   Latecy 100ms - 125ms = %s' . "\n", number_format(Stats::$latencyCounter[4]));
printf(' -   Latecy 125ms - 150ms = %s' . "\n", number_format(Stats::$latencyCounter[5]));
printf(' -   Latecy 150ms - 175ms = %s' . "\n", number_format(Stats::$latencyCounter[6]));
printf(' -   Latecy 175ms - 200ms = %s' . "\n", number_format(Stats::$latencyCounter[7]));
printf(' -   Latecy 200ms+        = %s' . "\n", number_format(Stats::$latencyCounter[8]));
