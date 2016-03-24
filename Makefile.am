include tests/Makefile.am

AM_CPPFLAGS         = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h
AM_CXXFLAGS         = -Wall -std=c++14 -ggdb3 

bin_PROGRAMS        = eclipse_node dfsput dfsget dfsls dfsformat dfsrm dfsshow


messages_files   = src/messages/boundaries.cc      \
                   src/messages/message.cc         \
                   src/messages/keyvalue.cc        \
                   src/messages/keyrequest.cc      \
                   src/messages/control.cc         \
                   src/messages/boost_impl.cc      \
                   src/messages/factory.cc         \
                   src/messages/fileinfo.cc        \
                   src/messages/blockinfo.cc       \
                   src/messages/task.cc            \
                   src/messages/reply.cc           \
                   src/messages/filerequest.cc     \
                   src/messages/blockrequest.cc    \
                   src/messages/filelist.cc        \
                   src/messages/filedescription.cc \
                   src/messages/cacheinfo.cc       \
                   src/messages/blockdel.cc        \
                   src/messages/filedel.cc         \
									 src/messages/formatrequest.cc   \
									 src/messages/fileexist.cc

# libs -----
lib_LTLIBRARIES     = libecfs.la
libecfs_la_SOURCES  = src/common/hash.cc src/common/settings.cc \
											src/common/util.cc src/common/logger.cc src/common/histogram.cc \
											src/common/context.cc src/common/dl_loader.cc

libecfs_la_LDFLAGS  = -version-info 0:0:0 

# Binaries ----
AM_LDFLAGS          = -static
LDADD               = libecfs.la -lboost_system -lboost_thread -lboost_serialization -lboost_system -lboost_coroutine -lboost_context


eclipse_node_SOURCES = src/targets/node_main.cc \
											 src/network/channel.cc \
											 src/network/asyncchannel.cc \
											 src/network/p2p.cc \
											 src/network/server.cc \
											 src/nodes/machine.cc \
											 src/nodes/peerdfs.cc \
											 src/nodes/remotedfs.cc \
											 src/nodes/router.cc \
											 src/fs/directory.cc \
											 src/nodes/node.cc \
											 $(messages_files)

eclipse_node_LDADD   = $(LDADD) -lsqlite3

dfsput_SOURCES       = src/fs/dfsput.cc src/fs/directory.cc \
											 $(messages_files)

dfsput_LDADD         = $(LDADD) -lsqlite3

dfsget_SOURCES       = src/fs/dfsget.cc src/fs/directory.cc \
											 $(messages_files)

dfsget_LDADD         = $(LDADD) -lsqlite3

dfsls_SOURCES        = src/fs/dfsls.cc src/fs/directory.cc \
											$(messages_files)

dfsls_LDADD          = $(LDADD) -lsqlite3

dfsformat_SOURCES      = src/fs/dfsformat.cc src/fs/directory.cc \
											$(messages_files)

dfsformat_LDADD        = $(LDADD) -lsqlite3

dfsrm_SOURCES      = src/fs/dfsrm.cc src/fs/directory.cc \
											$(messages_files)

dfsrm_LDADD        = $(LDADD) -lsqlite3

dfsshow_SOURCES      = src/fs/dfsshow.cc src/fs/directory.cc \
											$(messages_files)

dfsshow_LDADD        = $(LDADD) -lsqlite3

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
