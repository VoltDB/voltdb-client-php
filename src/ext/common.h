#ifndef VOLT_COMMON_H
#define VOLT_COMMON_H

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
}

/* Common functions */

/* Get varargs into an array. The caller must efree() the args. */
inline int get_varargs(int argc, zval ****args)
{
    if (argc < 1) {
        return 0;
    }

    *args = (zval ***)emalloc(argc * sizeof(zval **));
    php_printf("parsing varargs\n");
    if (zend_get_parameters_array_ex(argc, *args) == FAILURE) {
        php_printf("failed parsing varargs\n");
        efree(*args);
        return 0;
    }

    php_printf("parsed varargs\n");
    return 1;
}

#endif  // VOLT_COMMON_H
