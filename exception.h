/* This file is part of VoltDB.
 * Copyright (C) 2008-2017 VoltDB Inc.
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

#ifndef VOLTDB_EXCEPTION_H
#define VOLTDB_EXCEPTION_H

/* Code for each type of exception */
const int errException = 1;
const int errNullPointerException = 2;
const int errInvalidColumnException = 3;
const int errOverflowUnderflowException = 4;
const int errIndexOutOfBoundsException = 5;
const int errNonExpandableBufferException = 6;
const int errUninitializedParamsException = 7;
const int errParamMismatchException = 8;
const int errNoMoreRowsException = 9;
const int errStringToDecimalException = 10;
const int errConnectException = 11;
const int errNoConnectionsException = 12;
const int errLibEventException = 13;
const int errClusterInstanceMismatchException = 14;
const int errColumnMismatchException = 15;
const int errMisplacedClientException = 16;

#endif /* VOLTDB_EXCEPTION_H */
