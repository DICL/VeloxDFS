include tests/Makefile.am

# Global options, users will use CPPFLAGS and CXXFLAGS
AM_CPPFLAGS      = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h $(BOOST_CPPFLAGS)
AM_CXXFLAGS      = $(CXXFLAGS) -Wall -Werror

# Binaries that we will produce
bin_PROGRAMS     = eclipse_node veloxdfs velox_get_config
dist_bin_SCRIPTS = src/stats/read_io_stats.sh src/stats/scheduler.py src/targets/veloxd src/targets/veloxdfs-rm

messages_files =   src/messages/message.cc         \
                   src/messages/boost_impl.cc      \
                   src/messages/factory.cc         \
                   src/messages/fileinfo.cc        \
                   src/messages/fileupdate.cc      \
                   src/messages/blockinfo.cc       \
                   src/messages/reply.cc           \
                   src/messages/filerequest.cc     \
                   src/messages/blockrequest.cc    \
                   src/messages/filelist.cc        \
                   src/messages/filedescription.cc \
                   src/messages/filedel.cc         \
                   src/messages/formatrequest.cc   \
                   src/messages/fileexist.cc       \
                   src/messages/metadata.cc        \
                   src/messages/IOoperation.cc


# libs -----
lib_LTLIBRARIES     = libvdfs.la 
libvdfs_la_SOURCES  = $(messages_files) \
                      src/client/dfs.cc \
                      src/client/vdfs.cc \
                      src/client/dfs_zookeeper.cc \
                      src/common/hash.cc src/common/settings.cc \
                      src/common/logger.cc src/common/histogram.cc \
                      src/common/context.cc \
                      src/common/context_singleton.cc

libvdfs_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libvdfs_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl -lzookeeper_mt

# BOOST_STATIC only for environments where we cannot install all the dependencies in every node
if BOOST_STATIC
AM_LDFLAGS          = -static $(BOOST_LDFLAGS) -Wl,--start-group -Wl,-Bstatic,-lboost_system,-lboost_serialization,-lboost_coroutine,-lboost_thread,-lboost_context,-Bdynamic
LDADD               = libvdfs.la -lrt
else
AM_LDFLAGS          = $(BOOST_LDFLAGS)
LDADD               = libvdfs.la -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context
endif

# Main binary
eclipse_node_main    = src/targets/node_main.cc
eclipse_node_SOURCES = $(eclipse_node_main) \
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

eclipse_node_LDADD   = $(LDADD)

veloxdfs_SOURCES     = src/targets/client.cc \
                       src/client/cli_driver.cc

velox_get_config_SOURCES = src/targets/velox_get_config.cc

pkginclude_HEADERS  = src/client/vdfs.hh \
                      src/client/dfs.hh

modelincludedir     = $(pkgincludedir)/model
modelinclude_HEADERS = src/client/model/metadata.hh \
                       src/client/model/block_metadata.hh


if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
                      $(sh_scripts) doc/eclipse.json

if LOGICAL_BLOCKS_FEATURE

# Add eclipse_node code==============================================
eclipse_node_SOURCES+= src/stats/io_monitor_invoker.cc \
                       src/stats/zk_listener.cc \
                       src/stats/scheduler_factory.cc \
                       src/stats/scheduler_score_based.cc \
                       src/stats/scheduler_simple.cc \
                       src/stats/scheduler_python.cc \
                       src/stats/scheduler_vlmb.cc \
                       src/stats/scheduler_multiwave.cc \
                       src/stats/scheduler_lean.cc

eclipse_node_LDADD  +=  -lzookeeper_mt

# Compile JAVA classes ==============================================
AM_JAVACFLAGS = -cp $(top_srcdir)/lib/java/json-simple-1.1.jar
javadir = $(datadir)/java
dist_java_JAVA = src/java/velox/Configuration.java \
		             src/java/velox/VeloxDFS.java \
		             src/java/velox/model/BlockMetadata.java \
		             src/java/velox/model/Metadata.java


# CREATE JAR file      ==============================================
all-local: veloxdfs.jar

jar_DATA = veloxdfs.jar $(top_srcdir)/lib/java/json-simple-1.1.jar

jardir = $(libdir)/java
veloxdfs.jar: $(dist_java_JAVA) classjava.stamp
	javah -jni -d $(top_srcdir)/java/jni com.dicl.velox.VeloxDFS
	jar cf veloxdfs.jar com

# Create shared lib    ==============================================
# In lib_LTLIBRARIES we wrote libveloxdfs-jni.la now we change it to underscore.
# Reason found at: https://lists.gnu.org/archive/html/automake/2003-08/msg00096.html
lib_LTLIBRARIES    += libveloxdfs-jni.la
libveloxdfs_jni_la_SOURCES = src/java/jni/velox_VeloxDFS.h \
								             src/java/jni/com_dicl_velox_VeloxDFS.h \
								             src/java/jni/VeloxDFS.cc

libveloxdfs_jni_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libveloxdfs_jni_la_LIBADD   = libvdfs.la

endif
