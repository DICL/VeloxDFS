AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include unittest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a -ldl

# Input your tests files here
check_PROGRAMS   = nodes executor units

TESTS            = units 
check_SCRIPTS    = tests/nodes_executor.in   \
                   tests/eclipse_debug.in

check_DATA       = $(top_srcdir)/doc/eclipse.json
CLEANFILES       = eclipse.json

messages_files   = src/messages/boundaries.cc \
                   src/messages/message.cc    \
                   src/messages/keyvalue.cc   \
                   src/messages/keyrequest.cc \
                   src/messages/control.cc    \
                   src/messages/boost_impl.cc \
                   src/messages/factory.cc    \
                   src/messages/task.cc       \
                   src/messages/fileinfo.cc   \
                   src/mapreduce/task.cc       

# Input the dependencies of the test files here
units_LDADD        = $(LDADD_T)
units_CPPFLAGS     = $(AM_CPPFLAGS_T)
units_SOURCES      = $(messages_files) \
                     tests/main.cc \
                     tests/settings_tests.cc \
                     tests/cache_test.cc \
                     tests/messages_test.cc \
                     tests/fileio_test.cc \
                     tests/nodes_test.cc \
                     tests/dl_test.cc \
                     src/fs/fileio.cc \
                     src/common/dl_loader.cc \
                     src/network/channel.cc \
                     src/network/asyncchannel.cc \
                     src/network/p2p.cc \
                     src/nodes/machine.cc \
                     src/nodes/peer.cc \
                     src/nodes/node.cc 

nodes_LDADD     = $(LDADD_T)
nodes_CPPFLAGS  = $(AM_CPPFLAGS_T)
nodes_SOURCES   = tests/nodes.cc \
                  $(messages_files) \
                  src/network/channel.cc \
                  src/network/asyncchannel.cc \
                  src/network/p2p.cc \
                  src/nodes/machine.cc \
                  src/nodes/peer.cc \
                  src/nodes/node.cc 

executor_LDADD     = $(LDADD_T)
executor_CPPFLAGS  = $(AM_CPPFLAGS_T)
executor_SOURCES   = tests/executor.cc \
                     $(messages_files) \
                     src/network/channel.cc \
                     src/network/asyncchannel.cc \
                     src/network/server.cc \
                     src/network/p2p.cc \
                     src/mapreduce/executor.cc \
                     src/nodes/machine.cc \
                     src/nodes/peer.cc \
                     src/nodes/node.cc \
                     src/mapreduce/peermr.cc
