dnl This function will enable static linkage of the boost library in eclipse.
dnl This feature aim to ease the cumbersome task of installing boost library
dnl in a large cluster
AC_DEFUN([AX_BOOST_STATIC_LINKAGE], 
[

AC_ARG_ENABLE([boost_static],
  [AS_HELP_STRING([--enable-boost-static], [statically link boost])])

AM_CONDITIONAL(BOOST_STATIC, [test "x$enable_boost_static" = xyes])
])
