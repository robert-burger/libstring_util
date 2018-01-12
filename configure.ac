AC_INIT([libstring_util], [1.1.6], [florian.schmidt@dlr.de])
AM_INIT_AUTOMAKE([-Wall -Werror foreign silent-rules])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

m4_ifdef([RMPM_ENV], [RMPM_ENV], [AC_SUBST([ARCH_CFLAGS], [''])])
m4_ifdef([RMPM_ARCH], [RMPM_ARCH libdir='${exec_prefix}/lib'])

AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL
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
 test/example/Makefile
])

INSTALL_DATA='${INSTALL} -C -m 644'

AC_OUTPUT
