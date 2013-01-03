<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2013 VoltDB Inc.
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

/**
 * This script runs before tests are run and makes all test framework classes available.
 */

// PHPUnit framework
require_once('PHPUnit/Framework.php');

// wrapper interface
require('dist/voltdb.php');

// test utilities
require('test/util/ProjectBuilder.php');
require('test/util/SampleValues.php');
require('test/util/Server.php');
require('test/util/Shell.php');

// projects
require('test/projects/HelloWorld.php');
require('test/projects/SecuritySuite.php');
require('test/projects/TypeSuite.php');

// voltdb installation
define('VOLTDB', trim(file_get_contents('tmp/voltdb.txt')) . '/voltdb');
define('VOLTDBJAR', VOLTDB . '/voltdb-*.jar');
