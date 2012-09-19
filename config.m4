dnl $Id$
dnl config.m4 for extension voltdb

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(voltdb, for voltdb support,
[  --with-voltdb           Include voltdb support. DIR is the VoltDB C++
                          library base directory.])

if test "$PHP_VOLTDB" != "no"; then
  dnl Require C++ compiler
  PHP_REQUIRE_CXX()

  # search for VoltDB C++ client headers
  if [ ! test -r "$PHP_VOLTDB/include/Client.h" ]; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please check voltdb C++ client library])
  fi

  # Add C++ client library headers to the include path
  PHP_ADD_INCLUDE($PHP_VOLTDB/include)

  PHP_SUBST(VOLTDB_SHARED_LIBADD)
  AC_CANONICAL_HOST
  case "${host_os}" in
    darwin*)
            THIRD_PARTY_LIB=$PHP_VOLTDB/third_party_libs/osx
            ;;
    *)
            THIRD_PARTY_LIB=$PHP_VOLTDB/third_party_libs/linux
            PHP_ADD_LIBRARY(rt, 1, VOLTDB_SHARED_LIBADD)
            ;;
  esac

  PHP_ADD_LIBRARY(stdc++, 1, VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(event, $THIRD_PARTY_LIB, VOLTDB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(event_pthreads, $THIRD_PARTY_LIB, VOLTDB_SHARED_LIBADD)

  # The PHP client will segfault if compiled with compiler optimization,
  # removing the default -O2 here
  MY_CFLAGS=`echo $CFLAGS | tr " " "\n" | while read line
  do
          if test x"$line" != "x-O2"
          then
                  echo -n "$line "
          fi
  done`
  CFLAGS=$MY_CFLAGS
  MY_CXXFLAGS=`echo $CXXFLAGS | tr " " "\n" | while read line
  do
          if test x"$line" != "x-O2"
          then
                  echo -n "$line "
          fi
  done`
  CXXFLAGS=$MY_CXXFLAGS

  LDFLAGS="$LDFLAGS $PHP_VOLTDB/libvoltdbcpp.a"

  voltdb_sources="voltdb.cpp client.cpp response.cpp table.cpp"
  PHP_NEW_EXTENSION(voltdb,
                    $voltdb_sources,
                    $ext_shared,
                    ,
                    "-DBOOST_SP_DISABLE_THREADS -D__STDC_LIMIT_MACROS",
                    "yes")
fi
