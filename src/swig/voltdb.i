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
%feature("director");

/*
%feature("director:except") {
    if ($error == FAILURE) {
        throw voltdb::Exception();
    }
}
*/

%include "exception.i"
%exception {
    try {
      $action
    } catch (const std::exception& e) {
      SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

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
%}

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
        }
    }

}

/* Callback wrapper classes */

abstract class ProcedureCallback {

    public abstract function callback($response);

}

class ProcedureCallbackWrapper extends ProcedureCallbackNative {

    private $client;
    private $callback;
    private $index;

    public function __construct($client, $callback, $index) {
        parent::__construct();
        $this->client = $client;
        $this->callback = $callback;
        $this->index = $index;
    }

    public function callback($response) {
        $this->client->invoked($this->index);
        $retval = $this->callback->callback(new InvocationResponse($response));
        $response = null; // avoids memory leak
        return $retval === null ? false : $retval;
    }

}
"