AC_INIT([libstring_util], [1.0.2], [florian.schmidt@dlr.de])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

RMPM_ENV
RMPM_ARCH

AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
m4_ifdef([PKG_INSTALLDIR], [PKG_PROG_PKG_CONFIG])
m4_ifdef([PKG_INSTALLDIR], [PKG_INSTALLDIR])

AC_CHECK_HEADERS([windows.h])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 libstring_util.pc
 src/Makefile 
 include/Makefile 
])

AC_OUTPUT
