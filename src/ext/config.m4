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

dnl PHP_ARG_ENABLE(voltdb, whether to enable voltdb support,
dnl Make sure that the comment is aligned:
dnl [  --enable-voltdb           Enable voltdb support])

if test "$PHP_VOLTDB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-voltdb -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/voltdb.h"  # you most likely want to change this
  dnl if test -r $PHP_VOLTDB/$SEARCH_FOR; then # path given as parameter
  dnl   VOLTDB_DIR=$PHP_VOLTDB
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for voltdb files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VOLTDB_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VOLTDB_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the voltdb distribution])
  dnl fi

  dnl # --with-voltdb -> add include path
  dnl PHP_ADD_INCLUDE($VOLTDB_DIR/include)

  dnl # --with-voltdb -> check for lib and symbol presence
  dnl LIBNAME=voltdb # you may want to change this
  dnl LIBSYMBOL=voltdb # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VOLTDB_DIR/lib, VOLTDB_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VOLTDBLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong voltdb lib version or lib not found])
  dnl ],[
  dnl   -L$VOLTDB_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VOLTDB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(voltdb, voltdb.c, $ext_shared)
fi
