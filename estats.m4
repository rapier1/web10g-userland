
dnl AM_PATH_ESTATS([EXACT-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Estats, and define ESTATS_CFLAGS and ESTATS_LIBS
dnl
AC_DEFUN([AM_PATH_ESTATS],
[
estats_success=""

AC_PATH_PROG([ESTATS_CONFIG], [estats-config], [no])

AC_MSG_CHECKING(for Estats)

if test "$ESTATS_CONFIG" != "no"; then
  ESTATS_CFLAGS=`$ESTATS_CONFIG --cflags`
  ESTATS_LIBS=`$ESTATS_CONFIG --libs`
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  echo "*** The estats-config script installed by Estats could not be found"
  echo "*** If Estats was installed in PREFIX, make sure PREFIX/bin is in"
  echo "*** your path, or set the ESTATS_CONFIG environment variable to the"
  echo "*** full path to estats-config"
  estats_success="no"
fi

if test x$estats_success = x; then
  if test "x$1" != "x"; then
    AC_MSG_CHECKING(for Estats - version $1)

    ESTATS_VERSION=`$ESTATS_CONFIG --version`
    if test "$ESTATS_VERSION" = "$1"; then
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
      echo "*** The requested ($1) and installed ($ESTATS_VERSION) versions"
      echo "*** of Estats do not match."
      estats_success="no"
    fi
  fi
fi

if test x$estats_success = x; then
  estats_success="yes"
fi

if test x$estats_success = xyes; then
  m4_if([$2], [], [:], [$2])
else
  ESTATS_CFLAGS=""
  ESTATS_LIBS=""
  m4_if([$3], [], [:], [$3])
fi

AC_SUBST(ESTATS_CFLAGS)
AC_SUBST(ESTATS_LIBS)
])
