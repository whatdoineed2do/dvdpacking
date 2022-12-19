AC_PREREQ(2.64)
AC_INIT([dvdpacking], [1.0.2], [https://github.com/whatdoineed2do/dvdpacking/issues], [https://github.com/whatdoineed2do/dvdpacking], [whatdoineed2do@nospam-github.com])
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip])
AC_CONFIG_MACRO_DIRS([m4])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
AX_CXX_COMPILE_STDCXX([17], [noext], [optional])
AC_HEADER_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL


AC_ARG_ENABLE([17-readdir],
    AS_HELP_STRING([--enable-17-readdir], [Enable std 17 filesystem (no readdir(2))]),
)
AS_IF([test "x$enable_17_readdir" = "xyes" && test $HAVE_CXX17 = 1], [
    AC_DEFINE(CXX17_READDIR,1,[readdir])
])

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
	[enable debug (default is no)])],
	[no_debug=false],
	[no_debug=true])
if test "$no_debug" = true; then
        building_debug=no
        AC_DEFINE(NDEBUG,1,[debug])
else
        building_debug=yes
        AC_DEFINE(DEBUG,1,[debug])
fi


AC_OUTPUT([
Makefile
src/Makefile
])
