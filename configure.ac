AC_INIT([VeloxDFS], 
    [1.10.0],
    [vicente.bolea@gmail.com], 
    [], [dicl.unist.ac.kr])
AC_CONFIG_AUX_DIR([./.autotools_aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])
LT_INIT([dlopen])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile ])

AM_CONDITIONAL([DEFAULT_CXXFLAGS], [test -z "$CXXFLAGS"])
AM_COND_IF([DEFAULT_CXXFLAGS], [CXXFLAGS='-g3 -O0 -march=native'])

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])

# Arguments {{{
AC_ARG_ENABLE([samples],
    [AS_HELP_STRING([--disable-samples], [copy sample of eclipse.json])])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [enable debug code (default is no)])],
    [debug="$withval"], [debug=no])

AC_ARG_ENABLE([lblocks],
    [AS_HELP_STRING([--enable-lblocks], [enable logical blocks experimental feature (Needs zookeeper)])])

AM_CONDITIONAL(COPY_SAMPLES, [test "$enable_samples" != no ])
AM_CONDITIONAL(LOGICAL_BLOCKS_FEATURE, [test "x$enable_lblocks" = xyes])
AX_BOOST_STATIC_LINKAGE

#}}}
# Dependencies checkings {{{
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h sys/socket.h sys/time.h unistd.h])
AC_SEARCH_LIBS([pthread_create], [pthread])
AC_SEARCH_LIBS([sqlite3_exec], [sqlite3], [], [AC_MSG_ERROR([sqlite3 not found, please install it.])])

AX_BOOST_BASE([1.53.0], ,
              [AC_MSG_ERROR([Needs BOOST])])
have_unittest=no
PKG_CHECK_EXISTS([UnitTest++], [have_unittest=yes], [have_unittest=no])

#}}}
# JNI and javac {{{
if test "x$enable_lblocks" = xyes; then
  AX_JNI_INCLUDE_DIR
  for include in $JNI_INCLUDE_DIRS; do
    CPPFLAGS="$CPPFLAGS -I$include"
  done
fi

#}}}
# PATHS {{{
FULLY_EVAL(["$sysconfdir"], [sysconfdirfull])
AC_SUBST(sysconfdirfull)

FULLY_EVAL(["$bindir"], [bindirfull])
AC_SUBST(bindirfull)

FULLY_EVAL(["$libdir"], [libdirfull])
AC_SUBST(libdirfull)

AH_TEMPLATE([ECLIPSE_CONF_PATH],[Configuration files (ETC) for Eclipse])
AC_DEFINE_UNQUOTED([ECLIPSE_CONF_PATH], ["$sysconfdirfull"])

AH_TEMPLATE([ECLIPSE_HOME_CONF_PATH],[Home configuration files (ETC) for Eclipse])
AC_DEFINE_UNQUOTED([ECLIPSE_HOME_CONF_PATH], ["$HOME"])

AH_TEMPLATE([IO_STAT_SCRIPT],[IOSTATS path])
AC_DEFINE_UNQUOTED([IO_STAT_SCRIPT], ["${bindirfull}/read_io_stats.sh"])

AH_TEMPLATE([PY_SCHEDULER_SCRIPT],[PY scheduler path])
AC_DEFINE_UNQUOTED([PY_SCHEDULER_SCRIPT], ["${bindirfull}/scheduler.py"])

AS_IF([test "x$enable_lblocks" = xyes], [
      AH_TEMPLATE([LOGICAL_BLOCKS_FEATURE],["Enables logical blocks feature" ]) 
      AC_DEFINE_UNQUOTED([LOGICAL_BLOCKS_FEATURE], [1])
     ])
#}}}
#OUTPUT{{{
AC_OUTPUT
WARN([\
-------------------------------------------------

 $PACKAGE_NAME Version $PACKAGE_VERSION

 Paths & Settings
 -----
 Prefix...........: $prefix
 Sysconfdir(etc)..: $sysconfdirfull
 C++ Compiler.....: $CXX $CXXFLAGS $CPPFLAGS
 LIBS.............: $LIBS
 Copy samples?....: ${enable_samples:-NO}

 Dependencies
 ------------
 [Required]
 Have Boost?.............: ${HAVE_BOOST-YES}
 BOOST CPPFLAGS..........: $BOOST_CPPFLAGS
 BOOST LDFLAGS...........: $BOOST_LDFLAGS
 Static Boost linkage?...: ${enable_boost_static:-NO}

 [Optional]
 Have UnitTest++?.: $have_unittest
 
 Next, type:
 AS_HELP_STRING([make @<:@-j<processors>@:>@], [Processors indicate how many threads you want to use])
 AS_HELP_STRING([make install],                [Will install the project in: $prefix])
    
-------------------------------------------------]) #}}}
# vim: foldmethod=marker
