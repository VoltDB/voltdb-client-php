INSTRUCTIONS for voter_initialize_apache.php, voter_apache.php

This example issues 5 VoltDB database queries (voter sample
application 'votes') each time it is invoked. It demonstates
using the VoltDB PHP connection cache and use of the VoltDB
PHP asynchronous API.

To run this example:
   * Start a VoltDB server running the voter example.
     This example is shipped in the VoltDB distribution
     under examples/voter.

   * Edit voter_initialize_apache.php and voter_apache.php
     to specify the hostname(s) of your VoltDB server(s).

   * Load voter_initialize_apache.php. It will print "Initialized"
     on success or an error if unsuccessful.

   * Call voter_apache.php repeatedly, maybe using apache bench.


Performance tips:
   * Use a PHP opcode cache or PHP will be bottlenecked
     loading and parsing the VoltDB PHP wrapper script.

   * Use HTTP persistent connections (the -k option to
     apache bench) between your clients and Apache or
     you will be bottlenecked doing TCP/IP connection
     establishment on your Apache webserver.

   * Set apache bench client concurrency high enough
     to keep ab from stalling on responses. Try ~100.

   * Use the VoltDB PHP client pool, as illustrated
     in this example, or throughput will be bottlenecked
     by Apache to VoltDB TCP/IP client establishment.

Following this guidance, we commonly measure ~4k PHP page loads per
second, resulting in ~20k VoltDB txns per second, using a single
apache instance.
