AC_INIT([EclipseDFS], 
    m4_esyscmd_s([git describe --tags | grep -o "[[:digit:]]\.[[:digit:]]\.[[:digit:]]"]), 
    [vicente.bolea@gmail.com], 
    [], [dicl.unist.ac.kr])
AC_CONFIG_AUX_DIR([./.autotools_aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])
LT_INIT([dlopen])

#AC_CONFIG_MACRO_DIR([./m4/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile ])
AC_CONFIG_FILES([tests/nodes_executor],    [chmod +x tests/nodes_executor])
AC_CONFIG_FILES([tests/eclipse_debug],    [chmod +x tests/eclipse_debug])

AM_CONDITIONAL([DEFAULT_CXXFLAGS], [test -z "$CXXFLAGS"])
AM_COND_IF([DEFAULT_CXXFLAGS], [CXXFLAGS='-g -O0 -march=native'])

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])

# Arguments {{{
AC_ARG_ENABLE([samples],
    AS_HELP_STRING([--disable-samples], [copy sample of eclipse.json]))

# Configure options: --enable-debug[=no].
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [enable debug code (default is no)])],
    [debug="$withval"], [debug=no])

AM_CONDITIONAL(COPY_SAMPLES, [test "$enable_samples" != no ])
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
AC_SEARCH_LIBS([sqlite3_exec], [sqlite3])

AX_BOOST_BASE([1.53.0], ,
              [AC_MSG_ERROR([Needs BOOST])])
have_unittest=no
PKG_CHECK_EXISTS([UnitTest++], [have_unittest=yes], [have_unittest=no])

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
