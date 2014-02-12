dnl $Id$
dnl config.m4 for extension geohash

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(geohash, for geohash support,
dnl Make sure that the comment is aligned:
dnl [  --with-geohash             Include geohash support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(geohash, whether to enable geohash support,
dnl Make sure that the comment is aligned:
dnl [  --enable-geohash           Enable geohash support])

if test "$PHP_GEOHASH" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-geohash -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/geohash.h"  # you most likely want to change this
  dnl if test -r $PHP_GEOHASH/$SEARCH_FOR; then # path given as parameter
  dnl   GEOHASH_DIR=$PHP_GEOHASH
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for geohash files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       GEOHASH_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$GEOHASH_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the geohash distribution])
  dnl fi

  dnl # --with-geohash -> add include path
  dnl PHP_ADD_INCLUDE($GEOHASH_DIR/include)

  dnl # --with-geohash -> check for lib and symbol presence
  dnl LIBNAME=geohash # you may want to change this
  dnl LIBSYMBOL=geohash # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $GEOHASH_DIR/lib, GEOHASH_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_GEOHASHLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong geohash lib version or lib not found])
  dnl ],[
  dnl   -L$GEOHASH_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(GEOHASH_SHARED_LIBADD)

  PHP_NEW_EXTENSION(geohash, geohash.c, $ext_shared)
fi
