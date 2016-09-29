include tests/Makefile.am

AM_CPPFLAGS      = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h $(BOOST_CPPFLAGS)
AM_CXXFLAGS      = $(CXXFLAGS) -Wall

bin_PROGRAMS     = eclipse_node dfs

messages_files   = src/messages/boundaries.cc      \
                   src/messages/message.cc         \
                   src/messages/keyvalue.cc        \
                   src/messages/offsetkv.cc        \
                   src/messages/keyrequest.cc      \
                   src/messages/control.cc         \
                   src/messages/boost_impl.cc      \
                   src/messages/factory.cc         \
                   src/messages/task.cc            \
                   src/messages/reply.cc           \
                   src/messages/cacheinfo.cc       \
                   src/messages/checkexist.cc      \
                   src/messages/formatrequest.cc   \
                   src/messages/metadata.cc        \
                   src/messages/fileinfo.cc        \
                   src/messages/filelist.cc        \
                   src/messages/filerequest.cc     \
                   src/messages/filedescription.cc \
                   src/messages/filedel.cc         \
                   src/messages/fileupdate.cc      \
                   src/messages/blockinfo.cc       \
                   src/messages/blocklist.cc       \
                   src/messages/blockrequest.cc    \
                   src/messages/blockdel.cc        \
                   src/messages/blockupdate.cc     \
                   src/messages/sliceinfo.cc       \
                   src/messages/slicelist.cc       \
                   src/messages/slicerequest.cc    \
                   src/messages/slicedel.cc        \
                   src/messages/mdlistinfo.cc      \
                   src/messages/mdlistlist.cc      \
                   src/messages/mdlistrequest.cc   \
                   src/messages/mdlistdel.cc       \
                   src/messages/mdlistupdate.cc    \
                   src/messages/nodeinfo.cc        \
                   src/messages/nodelist.cc        \
                   src/messages/noderequest.cc     \
                   src/messages/nodedel.cc         \
                   src/messages/nodeupdate.cc      \
                   src/messages/nodedescription.cc

# libs -----
lib_LTLIBRARIES     = libecfs.la
libecfs_la_SOURCES  = src/common/hash.cc src/common/settings.cc \
                      src/common/logger.cc src/common/histogram.cc \
                      src/common/context.cc src/common/dl_loader.cc \
                      src/common/context_singleton.cc

libecfs_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0

# Binaries ----
if BOOST_STATIC
AM_LDFLAGS          = -static $(BOOST_LDFLAGS) -Wl,--start-group -Wl,-Bstatic,-lboost_system,-lboost_serialization,-lboost_coroutine,-lboost_thread,-lboost_context,-Bdynamic
LDADD               = libecfs.la -lrt
else
AM_LDFLAGS          = -static $(BOOST_LDFLAGS)
LDADD               = libecfs.la -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context
endif

eclipse_node_SOURCES = src/targets/node_main.cc \
                       src/network/channel.cc \
                       src/network/asyncchannel.cc \
                       src/network/p2p.cc \
                       src/network/server.cc \
                       src/network/acceptor.cc \
                       src/network/connector.cc \
                       src/nodes/machine.cc \
                       src/nodes/peerdfs.cc \
                       src/nodes/local_io.cc \
                       src/nodes/remotedfs.cc \
                       src/nodes/router.cc \
                       src/fs/directory.cc \
                       src/nodes/node.cc \
                       $(messages_files)

dfs_SOURCES         = src/fs/dfs.cc \
                      src/fs/directory.cc \
                      src/fs/main.cc \
                      $(messages_files)

dfs_CXXFLAGS        = $(AM_CXXFLAGS)

pkginclude_HEADERS  = src/common/ecfs.hh src/common/settings.hh

if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json
man1_MANS           = doc/eclipsefs.1

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
                      $(sh_scripts) doc/eclipse.json
