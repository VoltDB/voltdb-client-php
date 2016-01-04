/* This file is part of VoltDB.
 * Copyright (C) 2008-2016 VoltDB Inc.
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
#include "volttable.h"
#include "php_voltdb.h"
#include "exception.h"

// Initialize global variables
// ZEND_DECLARE_MODULE_GLOBALS(voltdb)

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
    PHP_RINIT(voltdb), /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(voltdb), /* Replace with NULL if there's nothing to do at request end */
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

    create_voltclient_class(module_number);
    create_voltresponse_class();
    create_volttable_class();

    // Set up all the exception codes as constants
    REGISTER_LONG_CONSTANT("VOLT_EXCEPTION", errException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_NULLPOINTEREXCEPTION", errNullPointerException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_INVALIDCOLUMNEXCEPTION", errInvalidColumnException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_OVERFLOWUNDERFLOWEXCEPTION", errOverflowUnderflowException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_INDEXOUTOFBOUNDSEXCEPTION", errIndexOutOfBoundsException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_NONEXPANDABLEBUFFEREXCEPTION", errNonExpandableBufferException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_UNINITIALIZEDPARAMSEXCEPTION", errUninitializedParamsException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_PARAMMISMATCHEXCEPTION", errParamMismatchException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_NOMOREROWSEXCEPTION", errNoMoreRowsException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_STRINGTODECIMALEXCEPTION", errStringToDecimalException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_CONNECTEXCEPTION", errConnectException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_NOCONNECTIONSEXCEPTION", errNoConnectionsException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_LIBEVENTEXCEPTION", errLibEventException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_CLUSTERINSTANCEMISMATCHEXCEPTION", errClusterInstanceMismatchException, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("VOLT_COLUMNMISMATCHEXCEPTION", errColumnMismatchException, CONST_CS | CONST_PERSISTENT);

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
