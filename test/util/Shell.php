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

class Shell {

    public static function exec($cmd, $out = '/dev/null') {
        exec($cmd . ' >> ' . $out . ' 2>&1 ');
    }

    public static function execAsync($cmd, $out) {
        exec($cmd . ' >> ' . $out . ' 2>&1 ' . '& echo $!', $output);
        $pid = (int) $output[0];
        return $pid;
    }

    public static function kill($pid) {
        self::exec('kill ' . $pid);

        // block until the process is gone
        $waitMax = 3000000; // 3s
        $waitInterval = 100000; // 100ms
        $waited = 0;
        while ($waited < $waitMax) {
            exec(sprintf('ps -p %d > /dev/null', $pid), $output, $retval);
            if ($retval === 1) {
                return;
            }
            usleep($waitInterval);
            $waited += $waitInterval;
        }
        // @codeCoverageIgnoreStart
        printf('Process could not be killed: %d' . "\n", $pid);
        // @codeCoverageIgnoreStop
    }

}