/* This file is part of VoltDB.
 * Copyright (C) 2008-2012 VoltDB Inc.
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

#ifndef VOLT_CLIENT_H
#define VOLT_CLIENT_H

// VoltClient
PHP_METHOD(VoltClient, __construct);
PHP_METHOD(VoltClient, __destruct);
PHP_METHOD(VoltClient, connect);
PHP_METHOD(VoltClient, addStoredProc);
PHP_METHOD(VoltClient, removeStoredProc);
PHP_METHOD(VoltClient, getStoredProcs);
PHP_METHOD(VoltClient, invoke);
PHP_METHOD(VoltClient, invokeAsync);
PHP_METHOD(VoltClient, drain);

struct voltclient_object {
    zend_object std;
    voltdb::Client client;
    std::map<const char *, voltdb::Procedure *> procedures;
};

void create_voltclient_class(void);

#endif  // VOLT_CLIENT_H
