AC_INIT([libstring_util], [1.0.11], [florian.schmidt@dlr.de])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_ifdef([RMPM_ENV], [RMPM_ENV])
m4_ifdef([RMPM_ARCH], [RMPM_ARCH libdir='${exec_prefix}/lib'])

AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
m4_ifdef([PKG_PROG_PKG_CONFIG], [PKG_PROG_PKG_CONFIG])
m4_ifdef([PKG_INSTALLDIR], [PKG_INSTALLDIR], [pkgconfigdir='${libdir}/pkgconfig' AC_SUBST([pkgconfigdir])])

AC_CHECK_HEADERS([windows.h])
AC_CHECK_FUNCS([snprintf])


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 libstring_util.pc
 libstring_util.pt
 src/Makefile 
 include/Makefile 
])

INSTALL_DATA='${INSTALL} -C -m 644'

AC_OUTPUT
