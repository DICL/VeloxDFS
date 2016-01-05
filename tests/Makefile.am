AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include UnitTest++/UnitTest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a

# Input your tests files here
check_PROGRAMS   = units #settings cache_test messages_test topology

TESTS            = $(check_PROGRAMS)
check_DATA       = $(top_srcdir)/doc/eclipse.json
CLEANFILES       = eclipse.json

messages_files   = src/messages/boundaries.cc \
                   src/messages/message.cc    \
                   src/messages/keyvalue.cc   \
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
	 									 $(messages_files) \
										 tests/topology_test.cc \
                     src/network/topology.cc \
                     src/network/ringtopology.cc

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
