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

#ifndef PHP_VOLTDB_H
#define PHP_VOLTDB_H

extern zend_module_entry voltdb_module_entry;
#define phpext_voltdb_ptr &voltdb_module_entry

#ifdef PHP_WIN32
#    define PHP_VOLTDB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_VOLTDB_API __attribute__ ((visibility("default")))
#else
#    define PHP_VOLTDB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(voltdb);
PHP_MSHUTDOWN_FUNCTION(voltdb);
PHP_RINIT_FUNCTION(voltdb);
PHP_RSHUTDOWN_FUNCTION(voltdb);
PHP_MINFO_FUNCTION(voltdb);

/*
  Declare any global variables you may need between the BEGIN
  and END macros here:
*/
/* ZEND_BEGIN_MODULE_GLOBALS(voltdb) */

/* ZEND_END_MODULE_GLOBALS(voltdb) */

/* In every utility function you add that needs to use variables
   in php_voltdb_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as VOLTDB_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define VoltG(v) TSRMG(voltdb_globals_id, zend_voltdb_globals *, v)
#else
#define VoltG(v) (voltdb_globals.v)
#endif

#endif  /* PHP_VOLTDB_H */
