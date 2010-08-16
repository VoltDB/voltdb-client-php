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

%module(directors="1") voltdb

// connection pooling
%minit {
    voltdb::onLoad();
}
%rshutdown {
    voltdb::onScriptEnd();
}
%mshutdown {
    voltdb::onUnload();
}

%feature("director");

%{
#include "Exception.hpp"
#include "WireType.h"
#include "Parameter.hpp"
#include "ByteBuffer.hpp"
#include "Decimal.hpp"
#include "ParameterSet.hpp"
#include "Procedure.hpp"
#include "InvocationResponse.hpp"
#include "ProcedureCallback.hpp"
#include "Client.h"
#include "Row.hpp"
#include "TableIterator.h"
#include "Table.h"
#include "StatusListener.h"
#include "Column.hpp"
#include "ConnectionPool.h"
%}

// exceptions
%define PHP_EXCEPTION(NAME)
%typemap(throws) NAME %{
    std::string name = "NAME";
    if (name.substr(0, 8).compare("voltdb::") == 0) {
        name = name.substr(8, name.length());
    }
    zend_throw_exception(
            zend_fetch_class(name.c_str(), name.length(), 0),
            const_cast<char*>($1.what()),
            0 TSRMLS_CC);
    return;
%}
%enddef
PHP_EXCEPTION(voltdb::Exception);
PHP_EXCEPTION(voltdb::NullPointerException);
PHP_EXCEPTION(voltdb::InvalidColumnException);
PHP_EXCEPTION(voltdb::OverflowUnderflowException);
PHP_EXCEPTION(voltdb::IndexOutOfBoundsException);
PHP_EXCEPTION(voltdb::NonExpandableBufferException);
PHP_EXCEPTION(voltdb::UninitializedParamsException);
PHP_EXCEPTION(voltdb::ParamMismatchException);
PHP_EXCEPTION(voltdb::NoMoreRowsException);
PHP_EXCEPTION(voltdb::StringToDecimalException);
PHP_EXCEPTION(voltdb::ConnectException);
PHP_EXCEPTION(voltdb::NoConnectionsException);
PHP_EXCEPTION(voltdb::LibEventException);

// types
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long int int64_t;

// string support
%include "std_string.i"

// vector support
%include "std_vector.i"
%template(Parameters) std::vector<voltdb::Parameter>;
%template(Tables) std::vector<voltdb::Table>;
%template(Columns) std::vector<voltdb::Column>;

%template(Int8Vector) std::vector<int8_t>;
%template(Int16Vector) std::vector<int16_t>;
%template(Int32Vector) std::vector<int32_t>;
%template(Int64Vector) std::vector<int64_t>;
%template(DoubleVector) std::vector<double>;
%template(StringVector) std::vector<std::string>;
//%template(TimestampVector) std::vector<int64_t>;
%template(DecimalVector) std::vector<voltdb::Decimal>;

// renames
%rename(ClientNative) Client;
%rename(StatusListenerNative) StatusListener;
%rename(ProcedureCallbackNative) ProcedureCallback;

%include "Exception.hpp"
%include "WireType.h"
%include "Parameter.hpp"
%include "ByteBuffer.hpp"
%include "Decimal.hpp"
%include "ParameterSet.hpp"
%include "Procedure.hpp"
%include "InvocationResponse.hpp"
%include "ProcedureCallback.hpp"
%include "Client.h"
%include "Row.hpp"
%include "TableIterator.h"
%include "Table.h"
%include "StatusListener.h"
%include "Column.hpp"
%include "ConnectionPool.h"

%pragma(php) code="
/* Client wrapper class */

class Client {

    private $native;
    private $listener;
    private $callbackIndex = 0;
    private $callbacks = array();

    private function __construct($native, $listener = null) {
        $this->native = $native;
        $this->listener = $listener;
    }

    public function createConnection($hostname, $username = '', $password = '', $port = 21212) {
        $this->native->createConnection($hostname, $username, $password, $port);
    }

    public function invoke($procedure) {
        return $this->native->invoke($procedure);
    }

    public function invokeAsync($procedure, $callback) {
        $wrapper = new ProcedureCallbackWrapper($this, $callback, $this->callbackIndex);
        $this->callbacks[$this->callbackIndex] = $wrapper;
        $this->callbackIndex++;
        return $this->native->invokeAsync($procedure, $wrapper);
    }

    public function invoked($index) {
        unset($this->callbacks[$index]);
    }

    public function uncaughtException($exception, $callback) {
        if (isset($this->listener)) {
            return $this->listener->uncaughtException($exception, $callback);
        } else {
            return false;
        }
    }

    public function runOnce() {
        return $this->native->runOnce();
    }

    public function run() {
        return $this->native->run();
    }

    public function drain() {
        return $this->native->drain();
    }

    public static function create($listener = null) {
        switch (func_num_args()) {
            case 0:
                return new Client(ClientNative::create());
            case 1:
                $wrapper = new StatusListenerWrapper($listener);
                return new Client(ClientNative::create($wrapper), $wrapper);
            default:
                print('Invalid argument count to Client::create()' . \"\\n\");
                return null;
        }
    }

    public static function createFromPool($hostname, $username = '', $password = '', $listener = null, $port = 21212) {
        switch (func_num_args()) {
            case 1:
            case 3:
                return new Client(ConnectionPool::pool()->acquireClient($hostname, $username, $password, $port));
            case 4:
            case 5:
                $wrapper = new StatusListenerWrapper($listener);
                return new Client(
                    ConnectionPool::pool()->acquireClient($hostname, $username, $password, $wrapper, $port),
                    $wrapper);
            default:
                print('Invalid argument count to Client::createFromPool()' . \"\\n\");
                return null;
        }
    }

}

/* StatusListener wrapper classes */

abstract class StatusListener {

    public abstract function uncaughtException($exception, $callback);
    public abstract function connectionLost($hostname, $connectionsLeft);
    public abstract function backpressure($hasBackpressure);

}

class StatusListenerWrapper extends StatusListenerNative {

    private $listener;

    public function __construct($listener) {
        parent::__construct();
        $this->listener = $listener;
    }

    public function uncaughtException($exception, $callback) {
        $retval = $this->listener->uncaughtException($exception, $callback);
        return $retval === null ? false : $retval;
    }

    public function connectionLost($hostname, $connectionsLeft) {
        $retval = $this->listener->connectionLost($hostname, $connectionsLeft);
        return $retval === null ? false : $retval;
    }

    public function backpressure($hasBackpressure) {
        $retval = $this->listener->backpressure($hasBackpressure);
        return $retval === null ? false : $retval;
    }

}

/* ProcedureCallback wrapper classes */

abstract class ProcedureCallback {

    public abstract function callback($response);

}

class ProcedureCallbackWrapper extends ProcedureCallbackNative {

    private $client;
    private $callback;
    private $index;

    public function uncaughtException($exception, $callback) {
        $this->listener->uncaughtException($exception, $callback);
    }

    public function __construct($client, $callback, $index) {
        parent::__construct();
        $this->client = $client;
        $this->callback = $callback;
        $this->index = $index;
    }

    public function callback($response) {
        $this->client->invoked($this->index);
        try {
            $retval = $this->callback->callback(new InvocationResponse($response));
        } catch (Exception $e) {
            return $this->client->uncaughtException($e, $this->callback);
        }
        $response = null; // avoids memory leak
        return $retval === null ? false : $retval;
    }

}
"