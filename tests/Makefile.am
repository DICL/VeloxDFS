AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include unittest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a -ldl

# Input your tests files here
check_PROGRAMS   = units network nodes centralizedtopology

TESTS            = units #tests/network_executor
check_SCRIPTS    = tests/network_executor.in \
                   tests/nodes_executor.in   \
                   tests/eclipse_debug.in

check_DATA       = $(top_srcdir)/doc/eclipse.json
CLEANFILES       = eclipse.json

messages_files   = src/messages/boundaries.cc \
                   src/messages/message.cc    \
                   src/messages/keyvalue.cc   \
                   src/messages/keyrequest.cc   \
                   src/messages/control.cc    \
                   src/messages/boost_impl.cc \
                   src/messages/factory.cc

# Input the dependencies of the test files here
units_LDADD        = $(LDADD_T)
units_CPPFLAGS     = $(AM_CPPFLAGS_T)
units_SOURCES      = tests/main.cc \
                     tests/settings_tests.cc \
                     tests/cache_test.cc \
                     tests/messages_test.cc \
                     tests/nodes_test.cc \
                     $(messages_files) \
                     tests/topology_test.cc \
                     src/network/topology.cc \
                     src/network/ringtopology.cc \
                     src/network/meshtopology.cc \
                     src/network/channel.cc \
                     src/nodes/peerlocal.cc \
                     src/nodes/peerremote.cc \
                     src/nodes/nodelocal.cc \
                     src/nodes/noderemote.cc \
                     src/nodes/node.cc \
                     src/common/dl_loader.cc \
                     tests/dl_test.cc \
                     src/fs/fileio.cc \
                     src/fs/fileinfo.cc \
                     tests/fileio_test.cc

network_LDADD     = $(LDADD_T)
network_CPPFLAGS  = $(AM_CPPFLAGS_T)
network_SOURCES   = tests/network.cc \
                    src/network/topology.cc \
                    src/network/ringtopology.cc \
                    src/network/meshtopology.cc \
                    src/network/channel.cc 
nodes_LDADD     = $(LDADD_T)
nodes_CPPFLAGS  = $(AM_CPPFLAGS_T)
nodes_SOURCES   = tests/nodes.cc \
                  $(messages_files) \
                  src/network/topology.cc \
                  src/network/ringtopology.cc \
                  src/network/meshtopology.cc \
                  src/network/channel.cc \
                  src/nodes/peerlocal.cc \
                  src/nodes/peerremote.cc \
                  src/nodes/nodelocal.cc \
                  src/nodes/noderemote.cc \
                  src/nodes/node.cc
centralizedtopology_LDADD     = $(LDADD_T)
centralizedtopology_CPPFLAGS  = $(AM_CPPFLAGS_T)
centralizedtopology_SOURCES   = tests/centralizedtopology.cc \
                                $(messages_files) \
                                src/network/topology.cc \
                                src/network/centralizedtopology.cc \
                                src/network/channel.cc 


#nodes_test_SOURCES    = nodes_test.cc \
#                        $(top_srcdir)/src/nodes/node.cc \
#                        $(top_srcdir)/src/nodes/nodelocal.cc \
#                        $(top_srcdir)/src/nodes/peerlocal.cc \
#                        $(top_srcdir)/src/nodes/noderemote.cc \
#                        $(top_srcdir)/src/nodes/peerremote.cc \
#                        $(messages_files)
#dht_test_SOURCES      = dht_test.cc \
#                        $(top_srcdir)/src/dht/DHTserver.cc \
#                        $(top_srcdir)/src/dht/DHTclient.cc $(messages_files)
#
