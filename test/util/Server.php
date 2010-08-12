<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2010 VoltDB L.L.C.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

class Server {

    private $catalog;
    private $deployment;
    private $log;

    private $running = false;
    private $pid;

    public function __construct($catalog, $deployment, $log) {
        $this->catalog = $catalog;
        $this->deployment = $deployment;
        $this->log = $log;
    }

    /**
     * Starts a VoltDB server and blocks until it is ready.
     */
    public function start() {
        $this->pid = Shell::execAsync(
            sprintf('java -Djava.library.path=%s -cp %s org.voltdb.VoltDB catalog %s deployment %s',
                VOLTDB, VOLTDBJAR, $this->catalog, $this->deployment),
            $this->log);
        $this->running = true;
        return $this->waitForInitialization();
    }

    /**
     * Waits until the server is initialized.
     * @return Returns true if the server successfully initializes. Returns false on timeout.
     */
    private function waitForInitialization() {
        $waitMax = 3000000; // 3s
        $waitInterval = 100000; // 100ms
        $waited = 0;
        while ($waited < $waitMax) {
            if (strpos(file_get_contents($this->log), 'Server completed initialization.')) {
                return true;
            }
            usleep($waitInterval);
            $waited += $waitInterval;
        }
        $this->running = false;
        printf('Server failed to initialize, see the log for more details: %s' . "\n", $this->log);
        return false;
    }

    public function stop() {
        if ($this->running) {
            exec('kill ' . $this->pid);
        }
    }

}