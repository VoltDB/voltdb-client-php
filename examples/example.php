<?php

$hostname = 'localhost';
$port = 32778;

// Use a client that has a cached connection from the pool.
try {
    $voltClient = new VoltClient();
    $voltClient->connect($hostname, '', '', $port);
    echo "Connected\n";

    /*
     * All params are strings. If the procedure was not called before, this will
     * also create a procedure with strings as param types.
     */
    $voltarr = new VoltArray(VoltArray::VOLT_TYPE_TIMESTAMP);
    $voltarr->add("1499463188000000", "1499473188000000");
    $resp = $voltClient->invoke("SelectWithin", array($voltarr));
    if ($resp === null) {
        echo "invoke had an error\n";
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
/*
    $voltClient->connect($hostname, '', '', $port);
    $resp = $voltClient->invoke("Vote", array(4, 5, 6));
    if ($resp === null) {
        echo "invoke had an error\n";
    } else {
        echo "invoke returned a response\n";
    }
    $voltClient->close();
    */
} catch (Exception $e) {
    echo "Exception " . $e->getCode() . " " . $e->getMessage() . "\n";
    return;
}

?>
