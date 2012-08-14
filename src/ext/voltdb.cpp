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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

#include "ConnectionPool.h"
#include "client.h"
#include "response.h"
#include "php_voltdb.h"

// Initialize global variables
// ZEND_DECLARE_MODULE_GLOBALS(voltdb)

/* True global resources - no need for thread safety here */
static int le_voltdb;

/* {{{ voltdb_functions[]
 *
 * Every user visible function must have an entry in voltdb_functions[].
 */
// const zend_function_entry voltdb_methods[] = {
//     // // VoltTable
//     // PHP_ME(VoltTable, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
//     // PHP_ME(VoltTable, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
//     // PHP_ME(VoltTable, iterator, NULL, ZEND_ACC_PUBLIC)
//     // PHP_ME(VoltTable, statusCode, NULL, ZEND_ACC_PUBLIC)
//     // PHP_ME(VoltTable, rowCount, NULL, ZEND_ACC_PUBLIC)
//     // PHP_ME(VoltTable, columnCount, NULL, ZEND_ACC_PUBLIC)
//     // PHP_ME(VoltTable, columns, NULL, ZEND_ACC_PUBLIC)

// 	PHP_FE_END	/* Must be the last line in voltdb_functions[] */
// };
/* }}} */

/* {{{ voltdb_module_entry
 */
zend_module_entry voltdb_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"voltdb",
	NULL,                       // Functions
	PHP_MINIT(voltdb),
	PHP_MSHUTDOWN(voltdb),
	PHP_RINIT(voltdb),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(voltdb),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(voltdb),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_VOLTDB
extern "C" {
ZEND_GET_MODULE(voltdb)
}
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("voltdb.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_voltdb_globals, voltdb_globals)
    STD_PHP_INI_ENTRY("voltdb.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_voltdb_globals, voltdb_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_voltdb_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_voltdb_init_globals(zend_voltdb_globals *voltdb_globals)
{
	voltdb_globals->global_value = 0;
	voltdb_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(voltdb)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

    create_voltclient_class();
    create_voltresponse_class();

    // Initialize the connection pool
    voltdb::onLoad();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(voltdb)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

    // Tear down the connection pool and any pooled clients
    voltdb::onUnload();

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(voltdb)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(voltdb)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(voltdb)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "voltdb support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
