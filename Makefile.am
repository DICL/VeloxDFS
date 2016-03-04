include tests/Makefile.am

AM_CPPFLAGS         = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h
AM_CXXFLAGS         = -Wall -std=c++14 -ggdb3 

bin_PROGRAMS        = dfsput dfsget dfsls dfsinit

# libs -----
lib_LTLIBRARIES     = libecfs.la
libecfs_la_SOURCES  = src/common/hash.cc src/common/settings.cc \
											src/common/util.cc src/common/logger.cc src/common/histogram.cc \
											src/common/context.cc src/common/dl_loader.cc
libecfs_la_LDFLAGS  = -version-info 0:0:0 

# Binaries ----
AM_LDFLAGS          = -static
LDADD               = libecfs.la -lboost_system -lboost_thread -lboost_serialization -lboost_system -lboost_coroutine -lboost_context


dfsput_SOURCES       = src/fs/dfsput.cc src/fs/directory.cc
dfsput_LDADD         = $(LDADD) -lsqlite3

dfsget_SOURCES       = src/fs/dfsget.cc src/fs/directory.cc
dfsget_LDADD         = $(LDADD) -lsqlite3

dfsls_SOURCES       = src/fs/dfsls.cc src/fs/directory.cc
dfsls_LDADD         = $(LDADD) -lsqlite3

dfsinit_SOURCES       = src/fs/dfsinit.cc src/fs/directory.cc
dfsinit_LDADD         = $(LDADD) -lsqlite3

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
