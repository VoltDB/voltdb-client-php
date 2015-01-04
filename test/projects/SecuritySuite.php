<?php

/* This file is part of VoltDB.
 * Copyright (C) 2008-2015 VoltDB Inc.
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

class SecuritySuite extends ProjectBuilder {

    public function __construct() {
        parent::__construct();

        $schemas = array();
        $schemas[] = '
            CREATE TABLE players (
                id      INTEGER NOT NULL,
                score   INTEGER NOT NULL,

                PRIMARY KEY (id)
            );
        ';
        parent::setSchemas($schemas);

        $procedures = array();
        $procedures[] = new ProcedureInfo(
            array(),
            'Insert',
            'INSERT INTO players VALUES (?, ?);',
            'players.id: 0');
        $procedures[] = new ProcedureInfo(
            array(),
            'Select',
            'SELECT score FROM players WHERE id = ?;',
            'players.id: 0');
        parent::setProcedures($procedures);

        $partitions = array();
        $partitions[] = new PartitionInfo('players', 'id');
        parent::setPartitions($partitions);

        $users = array();
        $users[] = new UserInfo('user-1', 'password-1', array('group-1'));
        $users[] = new UserInfo('user-2', 'password-2', array('group-2'));
        $users[] = new UserInfo('user-3', 'password-3', array('group-3'));
        $users[] = new UserInfo('user-4', 'password-4', array('group-4'));
        $users[] = new UserInfo('user-5', 'password-5', array());
        $users[] = new UserInfo('user-6', 'password-6', array('group-1', 'group-4'));
        parent::setUsers($users);

        $groups = array();
        $groups[] = new GroupInfo('group-1', false, false);
        $groups[] = new GroupInfo('group-2', false, true);
        $groups[] = new GroupInfo('group-3', true, false);
        $groups[] = new GroupInfo('group-4', true, true);
        parent::setGroups($groups);

        parent::setSecurityEnabled(true);
    }

}
