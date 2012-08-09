dnl $Id$
dnl config.m4 for extension voltdb

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(voltdb, for voltdb support,
dnl Make sure that the comment is aligned:
dnl [  --with-voltdb             Include voltdb support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(voltdb, whether to enable voltdb support,
Make sure that the comment is aligned:
[  --enable-voltdb           Enable voltdb support])

if test "$PHP_VOLTDB" != "no"; then
  dnl Require C++ compiler
  PHP_REQUIRE_CXX()

  # search for VoltDB C++ client headers
  SEARCH_PATH="../../../cpp"
  SEARCH_FOR="/include/Client.h"
  AC_MSG_CHECKING([for voltdb C++ client files in default path])
  for i in $SEARCH_PATH ; do
    if test -r $i/$SEARCH_FOR; then
      VOLTDB_CXX_DIR=$i
      AC_MSG_RESULT(found in $i)
    fi
  done

  if test -z "$VOLTDB_CXX_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please check voltdb C++ client library])
  fi

  # Add C++ client library headers to the include path
  PHP_ADD_INCLUDE($VOLTDB_CXX_DIR/include)

  # Check C++ client library
  CXX_LIBNAME=voltdbcpp
  # CXX_LIBSYMBOL=voltdb::Client

  # PHP_CHECK_LIBRARY($CXX_LIBNAME,$CXX_LIBSYMBOL,
  # [
  #   PHP_ADD_LIBRARY_WITH_PATH($CXX_LIBNAME, $VOLTDB_CXX_DIR, VOLTDB_SHARED_LIBADD)
  # ],[
  #   AC_MSG_ERROR([wrong voltdb C++ client lib version or lib not found])
  # ],[
  #   -L$VOLTDB_CXX_DIR -lm
  # ])

  PHP_SUBST(VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH($CXX_LIBNAME, $VOLTDB_CXX_DIR, VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(event, $VOLTDB_CXX_DIR/third_party_libs/linux, VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(event_pthreads, $VOLTDB_CXX_DIR/third_party_libs/linux, VOLTDB_SHARED_LIBADD)

  voltdb_sources="voltdb.cpp"
  PHP_NEW_EXTENSION(voltdb,
                    $voltdb_sources,
                    $ext_shared,
                    ,
                    "-DBOOST_SP_DISABLE_THREADS -D__STDC_LIMIT_MACROS",
                    "yes")
fi
