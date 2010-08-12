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

/**
 * PHP version of the Java VoltProjectBuilder programmatic interface for building VoltDB projects. This is used by test
 * cases to build projects to run against. Supports a subset of the actual project/deployment file features available
 * (no exports, for example). Also only supports single-server instances.
 */
abstract class ProjectBuilder {

    private $name;
    private $schemas;
    private $procedures;
    private $users;
    private $groups;
    private $partitions;
    private $security;

    private $server;

    public function __construct() {
        $backtrace = debug_backtrace();
        $caller = sprintf('%s-%s', $backtrace[2]['class'], $backtrace[2]['function']); // TestClass-MethodName
        $this->name = $caller;
        mkdir($this->getWorkingDir());
    }

    public function setSchemas(array $schemas) {
        $schemaIndex = 0;
        foreach ($schemas as $schema) {
            $name = 'schema' . (count($schemas) !== 1 ? ('-' . $schemaIndex++) : '') . '.sql';
            file_put_contents($this->getWorkingDir() . '/' . $name, $schema);
            $this->schemas[] = $name;
        }
    }

    public function setProcedures(array $procedures) {
        $this->procedures = $procedures;
    }

    public function setUsers(array $users) {
        $this->users = $users;
    }

    public function setGroups(array $groups) {
        $this->groups = $groups;
    }

    public function setPartitions(array $partitions) {
        $this->partitions = $partitions;
    }

    public function setSecurityEnabled($security) {
        $this->security = $security;
    }

    private function getWorkingDir() {
        return getcwd() . '/tmp/' . $this->name;
    }

    public function start() {
        if (!isset($this->name)) {
            // @codeCoverageIgnoreStart
            print('ProjectBuilder constructor must be explicitly called by subclasses.');
            exit(1);
            // @codeCoverageIgnoreStop
        }

        $project = $this->project();
        $catalog = $this->getWorkingDir() . '/catalog.jar';
        $log = $this->getWorkingDir() . '/log.txt';
        $this->compile($project, $catalog, $log);
        $deployment = $this->deployment();
        $this->server = new Server($catalog, $deployment, $log);
        $this->server->start();
    }

    private function project() {
        $project = $this->getWorkingDir() . '/project.xml';

        $writer = new XMLWriter();
        $writer->openURI($project);
        $writer->setIndent(true);
        $writer->setIndentString('    ');

        $writer->startDocument('1.0');
        $writer->startElement('project');

        $writer->startElement('security');
        $writer->writeAttribute('enabled', $this->security ? 'true' : 'false');
        $writer->endElement(); // security

        $writer->startElement('database');
        $writer->writeAttribute('name', 'database');

        $writer->startElement('schemas');
        foreach ($this->schemas as $schema) {
            $writer->startElement('schema');
            $writer->writeAttribute('path', $schema);
            $writer->endElement(); // schema
        }
        $writer->endElement(); // schemas

        $writer->startElement('procedures');
        foreach ($this->procedures as $procedure) {
            $writer->startElement('procedure');
            $writer->writeAttribute('class', $procedure->getName());
            if (count($procedure->getGroups()) > 0) {
                $writer->writeAttribute('groups', implode(',', $procedure->getGroups()));
            }
            $writer->writeAttribute('partitioninfo', $procedure->getPartitionInfo());
            $writer->writeElement('sql', $procedure->getSQL());
            $writer->endElement(); // procedure
        }
        $writer->endElement(); // procedures;

        $writer->startElement('partitions');
        foreach ($this->partitions as $partition) {
            $writer->startElement('partition');
            $writer->writeAttribute('table', $partition->getTable());
            $writer->writeAttribute('column', $partition->getColumn());
            $writer->endElement(); // partition
        }
        $writer->endElement(); // partitions

        if (count($this->groups) > 0) {
            $writer->startElement('groups');
            foreach ($this->groups as $group) {
                $writer->startElement('group');
                $writer->writeAttribute('name', $group->getName());
                $writer->writeAttribute('adhoc', $group->getAdhoc() ? 'true' : 'false');
                $writer->writeAttribute('sysproc', $group->getSysproc() ? 'true' : 'false');
                $writer->endElement(); // group
            }
            $writer->endElement(); // groups;
        }

        $writer->endElement(); // database
        $writer->endElement(); // project
        $writer->flush();

        return $project;
    }

    private function compile($project, $catalog, $log) {
        Shell::exec(
            sprintf('java -cp %s org.voltdb.compiler.VoltCompiler %s %s', VOLTDBJAR, $project, $catalog),
            $log);
    }

    private function deployment() {
        $deployment = $this->getWorkingDir() . '/deployment.xml';

        $writer = new XMLWriter();
        $writer->openURI($deployment);
        $writer->setIndent(true);
        $writer->setIndentString('    ');

        $writer->startDocument('1.0');
        $writer->startElement('deployment');

        $writer->startElement('cluster');
        $writer->writeAttribute('hostcount', 1);
        $writer->writeAttribute('sitesperhost', 2);
        $writer->writeAttribute('leader', 'localhost');
        $writer->writeAttribute('kfactor', 0);
        $writer->endElement(); // cluster

        if (count($this->users) > 0) {
            $writer->startElement('users');
            foreach ($this->users as $user) {
                $writer->startElement('user');
                $writer->writeAttribute('name', $user->getName());
                $writer->writeAttribute('password', $user->getPassword());
                if (count($user->getGroups()) > 0) {
                    $writer->writeAttribute('groups', implode(',', $user->getGroups()));
                }
                $writer->endElement(); // user
            }
            $writer->endElement(); // users
        }

        $writer->endElement(); // deployment
        $writer->flush();

        return $deployment;
    }

    public function stop() {
        $this->server->stop();
    }

}

class SchemaInfo {

    private $schemaText;

    public function __construct($schemaText) {
        $this->schemaText = $schemaText;
    }

    public function getSchemaText() {
        return $this->schemaText;
    }

}

class ProcedureInfo {

    private $groups;
    private $name;
    private $sql;
    private $partitionInfo;

    public function __construct(array $groups, $name, $sql, $partitionInfo) {
        $this->groups = $groups;
        $this->name = $name;
        $this->sql = $sql;
        $this->partitionInfo = $partitionInfo;
    }

    public function getGroups() {
        return $this->groups;
    }

    public function getName() {
        return $this->name;
    }

    public function getSQL() {
        return $this->sql;
    }

    public function getPartitionInfo() {
        return $this->partitionInfo;
    }

}

class PartitionInfo {

    private $table;
    private $column;

    public function __construct($table, $column) {
        $this->table = $table;
        $this->column = $column;
    }

    public function getTable() {
        return $this->table;
    }

    public function getColumn() {
        return $this->column;
    }

}

class UserInfo {

    private $name;
    private $password;
    private $groups;

    public function __construct($name, $password, array $groups) {
        $this->name = $name;
        $this->password = $password;
        $this->groups = $groups;
    }

    public function getName() {
        return $this->name;
    }

    public function getPassword() {
        return $this->password;
    }

    public function getGroups() {
        return $this->groups;
    }

}

class GroupInfo {

    private $name;
    private $adhoc;
    private $sysproc;

    public function __construct($name, $adhoc, $sysproc) {
        $this->name = $name;
        $this->adhoc = $adhoc;
        $this->sysproc = $sysproc;
    }

    public function getName() {
        return $this->name;
    }

    public function getAdhoc() {
        return $this->adhoc;
    }

    public function getSysproc() {
        return $this->sysproc;
    }

}