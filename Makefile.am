include tests/Makefile.am

AM_CPPFLAGS         = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h
AM_CXXFLAGS         = -Wall -std=c++14 -ggdb3 -lsqlite3

#bin_PROGRAMS        = slave
bin_PROGRAMS        = dfsput

# libs -----
lib_LTLIBRARIES     = libecfs.la
libecfs_la_SOURCES  = src/common/hash.cc src/common/settings.cc \
											src/common/util.cc src/common/logger.cc src/common/histogram.cc \
											src/common/context.cc
libecfs_la_LDFLAGS  = -version-info 0:0:0 

# Binaries ----
AM_LDFLAGS          = -static
LDADD               = libecfs.la -lboost_system -lboost_thread -lboost_serialization -lboost_system -lboost_coroutine -lboost_context

#slave_SOURCES       = src/targets/slave_main.cc src/nodes/node.cc src/nodes/nodelocal.cc \
#                      src/nodes/peerlocal.cc src/nodes/peerremote.cc \
#                      src/nodes/noderemote.cc \
#                      src/messages/boost_impl.cc src/messages/boundaries.cc \
#                      src/messages/control.cc src/messages/factory.cc \
#                      src/messages/keyvalue.cc src/messages/message.cc

dfsput_SOURCES       = src/fs/dfsput.cc src/fs/directory.cc

#master_SOURCES      = src/master/master.cc 

pkginclude_HEADERS  = src/common/ecfs.hh src/common/settings.hh

# Scripts -------
#bin_SCRIPTS         = src/fs/eclipsed
# ------
if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json
man1_MANS           = doc/eclipsefs.1

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
											$(sh_scripts) doc/eclipse.json
