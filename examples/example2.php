<?php

$hostname = 'localhost';
$port = 32768;

// Use a client that has a cached connection from the pool.
try {
    $voltClient = new VoltClient();
    $voltClient->connect($hostname, '', '', $port);
    echo "Connected\n";

    /*
     * All params are strings. If the procedure was not called before, this will
     * also create a procedure with strings as param types.
     */
    $handle = $voltClient->invokeAsync("Vote", array(3, 4, 5));
    $retry = 0;
    while (!$voltClient->drain()) {
        $retry++;
    }
    echo $retry . "\n";
    echo $handle . "\n";
    $resp = $voltClient->getResponse($handle);
    if ($resp === null) {
        echo "invoke had an error\n";
        return;
    } else {
        echo "invoke returned a response\n";
    }

    if ($resp->statusCode() != VoltInvocationResponse::SUCCESS) {
        echo "status " . $resp->statusCode() . " " . $resp->statusString() . "\n";
    } else {
        echo "Success!!!\n";

        /* Iterate through all returned tables */
        while ($resp->hasMoreResults()) {
            $t = $resp->nextResult();
            echo "Next result " . $t->statusCode() . " row count " . $t->rowCount() . "\n";

            /* Iterate through all rows in the table */
            while ($t->hasMoreRows()) {
                $r = $t->nextRow();
                /* The returned row is a PHP array, print it */
                print_r($r);
            }
        }
    }
    $voltClient->close();
} catch (Exception $e) {
    echo "Exception " . $e->getCode() . "\n";
    return;
}

?>
