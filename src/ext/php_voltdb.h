/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VOLTDB_H
#define PHP_VOLTDB_H

extern zend_module_entry voltdb_module_entry;
#define phpext_voltdb_ptr &voltdb_module_entry

#ifdef PHP_WIN32
#	define PHP_VOLTDB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_VOLTDB_API __attribute__ ((visibility("default")))
#else
#	define PHP_VOLTDB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

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

// // VoltProcedure
// PHP_METHOD(VoltProcedure, __construct);
// PHP_METHOD(VoltProcedure, __destruct);
// PHP_METHOD(VoltProcedure, getParams);

// // VoltInvocationResponse
// PHP_METHOD(VoltInvocationResponse, __construct);
// PHP_METHOD(VoltInvocationResponse, __destruct);
// PHP_METHOD(VoltInvocationResponse, statusCode);
// PHP_METHOD(VoltInvocationResponse, statusString);
// PHP_METHOD(VoltInvocationResponse, appStatusCode);
// PHP_METHOD(VoltInvocationResponse, appStatusString);
// PHP_METHOD(VoltInvocationResponse, results);

// // VoltTable
// PHP_METHOD(VoltTable, __construct);
// PHP_METHOD(VoltTable, __destruct);
// PHP_METHOD(VoltTable, iterator);
// PHP_METHOD(VoltTable, statusCode);
// PHP_METHOD(VoltTable, rowCount);
// PHP_METHOD(VoltTable, columnCount);
// PHP_METHOD(VoltTable, columns);

PHP_MINIT_FUNCTION(voltdb);
PHP_MSHUTDOWN_FUNCTION(voltdb);
PHP_RINIT_FUNCTION(voltdb);
PHP_RSHUTDOWN_FUNCTION(voltdb);
PHP_MINFO_FUNCTION(voltdb);

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(voltdb)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(voltdb)
*/

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

#endif	/* PHP_VOLTDB_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
