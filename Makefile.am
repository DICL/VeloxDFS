include tests/Makefile.am

AM_CPPFLAGS      = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h $(BOOST_CPPFLAGS)
AM_CXXFLAGS      = $(CXXFLAGS) -Wall

bin_PROGRAMS     = eclipse_node veloxdfs

messages_files   = src/messages/boundaries.cc      \
                   src/messages/message.cc         \
                   src/messages/keyvalue.cc        \
                   src/messages/offsetkv.cc        \
                   src/messages/keyrequest.cc      \
                   src/messages/control.cc         \
                   src/messages/boost_impl.cc      \
                   src/messages/factory.cc         \
                   src/messages/fileinfo.cc        \
                   src/messages/fileupdate.cc      \
                   src/messages/blockinfo.cc       \
                   src/messages/blockupdate.cc     \
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
                   src/messages/fileexist.cc       \
                   src/messages/metadata.cc        \
                   src/messages/blockstatus.cc     \
                   src/messages/IOoperation.cc


# libs -----
lib_LTLIBRARIES     = libvdfs.la
libvdfs_la_SOURCES  = $(messages_files) \
                      src/client/dfs.cc \
                      src/client/vdfs.cc \
                      src/common/hash.cc src/common/settings.cc \
                      src/common/logger.cc src/common/histogram.cc \
                      src/common/context.cc src/common/dl_loader.cc \
                      src/common/context_singleton.cc

libvdfs_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libvdfs_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl

# Binaries ----
if BOOST_STATIC
AM_LDFLAGS          = -static $(BOOST_LDFLAGS) -Wl,--start-group -Wl,-Bstatic,-lboost_system,-lboost_serialization,-lboost_coroutine,-lboost_thread,-lboost_context,-Bdynamic
LDADD               = libvdfs.la -lrt
else
AM_LDFLAGS          = $(BOOST_LDFLAGS)
LDADD               = libvdfs.la -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context
endif

eclipse_node_SOURCES = src/targets/node_main.cc \
                       src/network/asyncchannel.cc \
                       src/network/server.cc \
                       src/network/client_handler.cc \
                       src/network/server_handler.cc \
                       src/network/simple_router.cc \
                       src/network/router_decorator.cc \
                       src/nodes/machine.cc \
                       src/nodes/node.cc \
                       src/blocknode/local_io.cc \
                       src/blocknode/block_node.cc \
                       src/blocknode/block_node_router.cc \
                       src/fileleader/directory.cc \
                       src/fileleader/file_leader.cc \
                       src/fileleader/file_leader_router.cc


veloxdfs_SOURCES         = src/targets/client.cc \
    		                   src/client/cli_driver.cc

pkginclude_HEADERS  = src/client/vdfs.hh \
                      src/client/dfs.hh

modelincludedir     = $(pkgincludedir)/model
modelinclude_HEADERS = src/client/model/metadata.hh \
												src/client/model/block_metadata.hh


if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json
man1_MANS           = doc/eclipsefs.1

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
                      $(sh_scripts) doc/eclipse.json
