AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include unittest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a -ldl -lsqlite3

# Input your tests files here
check_PROGRAMS   = nodes units metadata

TESTS            = units
check_SCRIPTS    = tests/nodes_executor.in   \
                   tests/eclipse_debug.in

check_DATA       = $(top_srcdir)/doc/eclipse.json
CLEANFILES       = eclipse.json


# Input the dependencies of the test files here
units_LDADD        = $(LDADD_T)
units_CPPFLAGS     = $(AM_CPPFLAGS_T)
units_SOURCES      = $(messages_files) \
                     tests/main.cc \
                     tests/messages_test.cc \
                     tests/dl_test.cc \
                     src/common/dl_loader.cc \
                     src/network/channel.cc \
                     src/network/asyncchannel.cc \
                     src/network/p2p.cc \
										 src/network/acceptor.cc \
										 src/network/connector.cc \
                     src/nodes/machine.cc \
                     src/nodes/node.cc 

nodes_LDADD     = $(LDADD_T)
nodes_CPPFLAGS  = $(AM_CPPFLAGS_T)
nodes_SOURCES   = tests/nodes.cc \
                  $(messages_files) \
                  src/network/channel.cc \
                  src/network/asyncchannel.cc \
                  src/network/p2p.cc \
                  src/network/server.cc \
                  src/network/acceptor.cc \
                  src/network/connector.cc \
                  src/nodes/machine.cc \
                  src/nodes/peerdfs.cc \
                  src/nodes/remotedfs.cc \
                  src/nodes/router.cc \
                  src/fs/directory.cc \
                  src/nodes/node.cc 

metadata_LDADD     = $(LDADD_T)
metadata_CPPFLAGS  = $(AM_CPPFLAGS_T)
metadata_SOURCES   = tests/metadata_test.cc \
                     src/fs/directory.cc \
                     $(messages_files)
