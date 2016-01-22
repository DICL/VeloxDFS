dnl 
dnl USAGE: just call ECLIPSE_FIND_BOOST no arguments
dnl 
dnl AUTHOR: Vicente Adolfo Bolea Sanchez
dnl
AC_DEFUN([ECLIPSE_FIND_BOOST], 
[
# BOOST LIB {{{
is_supported=yes

if test "$boost_path" == no ; then
  AC_MSG_CHECKING([for Boost path which isnot specified, trying to guessing OS])
  OS=`cat /etc/*release | sed -rn 's/.*(Ubuntu|CentOS).*/\1/p; q'`
  case $OS in
    Ubuntu) ;;
    CentOS) boost_path=`file /usr/local/include/boost* | cut -d ":" -f 1`
            CPPFLAGS="-I $boost_path"
            ;;
    *)      is_supported=no ;;
  esac 
fi
AC_MSG_RESULT([$OS])

have_boost=no
AC_CHECK_HEADERS([boost/foreach.hpp \
                  boost/property_tree/json_parser.hpp \
                  boost/property_tree/exceptions.hpp], [have_boost=yes])

if test "${have_boost}" = "no"; then
  AC_MSG_ERROR([
-------------------------------------------------
 I cannot find where you have the boost header files...
 OS supported?...: $is_supported
 Our OS guess?...: ${OS:-BoostPathSpecified}
 booth path......: $boost_path
 
 Re-run configure script in this way:

   $ ./configure --with-boost=/path/to/boost
   or
   $ CPLUS_INCLUDE_PATH=/path/to/boost ./configure

-------------------------------------------------])
fi
unset is_supported
# }}}
])
