<?php

$voltarr = new VoltArray(VoltArray::VOLT_TYPE_INTEGER);
$voltarr->add("11", array("aa", "bb", "cc"), "22", "33", "44", "55");
$voltarr->dump();

?>
