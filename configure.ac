AC_PREREQ([2.68])
AC_INIT([Web10G-userland], [2.0.7], [web10g-user@web10g.org])

dnl libestats libtool versioning
LIBESTATS_LT_CURRENT=2
LIBESTATS_LT_REVISION=0
LIBESTATS_LT_AGE=1

AC_SUBST(LIBESTATS_LT_CURRENT)
AC_SUBST(LIBESTATS_LT_REVISION)
AC_SUBST(LIBESTATS_LT_AGE)
AC_CONFIG_MACRO_DIRS([build-aux])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([])

AC_CONFIG_HEADERS([config.h])

dnl Check for programs
AC_CANONICAL_HOST
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_LIBTOOL
LT_INIT

AC_LANG([C])
AC_PROG_CC
AC_C_TYPEOF

AC_LANG([C++])
AC_PROG_CXX

dnl For Qt
#AX_CXX_COMPILE_STDCXX_11

dnl AutoTroll with Qt.
#AT_WITH_QT([], [], [], [want_gui=no], [want_gui=yes])

#AM_CONDITIONAL([COND_GUI], [test "$want_gui" = yes])

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [turn on debugging compiler options]),
              enable_debug=$enableval,
              enable_debug=no)
AC_ARG_ENABLE(warnings,
              AC_HELP_STRING([--enable-warnings],
                             [turn on compile warnings]),
              enable_warnings=$enableval,
              enable_warnings=no)

ESTATS_INCLUDE_DIR=${includedir}
AC_SUBST(ESTATS_INCLUDE_DIR)

# Checks for libraries.

have_libmnl="no"
PKG_CHECK_MODULES(LIBMNL, libmnl, [have_libmnl=yes], [have_libmnl=no])
if (test "${have_libmnl}" = "yes"); then
	AC_DEFINE(HAVE_LIBMNL, 1, [Define if you require specific liibmnl support])
	LIBMNL_CFLAGS="$LIBMNL_CFLAGS"
	LIBMNL_LIBS="$LIBMNL_LIBS"
	libmnl_version="1"
	have_libmnl="yes"
fi

if (test "${have_libmnl}" = "no"); then
	AC_MSG_ERROR([libmnl development header are required])
fi
AC_SUBST(LIBMNL_CFLAGS)
AC_SUBST(LIBMNL_LIBS)

# Checks for typedefs, structures, and compiler characteristics.

if ! test "x$GCC" = "xyes"; then
    AC_MSG_ERROR(compiling Web10G requires GCC)
fi

if test "x$enable_debug" = "xyes"; then
    if test -z "`echo $CFLAGS | grep "\-g" 2> /dev/null`"; then
        CFLAGS="$CFLAGS -g"
    fi
    CFLAGS=`echo $CFLAGS | sed -e 's/-O2//'`
    AC_DEFINE([DEBUG], 1,
              [Define to enable debugging when building.])
else
    AC_DEFINE([NDEBUG], 1,
              [Define to disable debugging (including asserts) when building.])
fi

if test "x$enable_warnings" = "xyes"; then
    for i in -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wextra; do
        if test -z "`echo $CFLAGS | grep "$i" 2> /dev/null`"; then
            CFLAGS="$CFLAGS $i"
        fi
    done
fi

if test "x$enable_warnings_as_errors" = "xyes"; then
    if test -z "`echo $CFLAGS | grep "\-Werror" 2> /dev/null`"; then
        CFLAGS="$CFLAGS -Werror"
    fi
fi

dnl GNU make allows us to use the $(strip ...) builtin which eliminates a
dnl large amount of extra whitespace in compile lines.
AC_MSG_CHECKING(whether make is GNU make)
STRIP_BEGIN=
STRIP_END=
if $ac_make --version 2> /dev/null | grep '^GNU Make ' > /dev/null ; then
    STRIP_BEGIN='$(strip $(STRIP_DUMMY)'
    STRIP_END=')'
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi
dnl some Make 3.79 $(strip ) versions are broken and require an empty arg
STRIP_DUMMY=
AC_SUBST(STRIP_DUMMY)
AC_SUBST(STRIP_BEGIN)
AC_SUBST(STRIP_END)

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/estats/Makefile
		 util/Makefile
		 util/C/Makefile
		 ])

AC_OUTPUT
