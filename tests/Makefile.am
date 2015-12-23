AM_CPPFLAGS      = -I@top_srcdir@/src/common -I@top_srcdir@/src
AM_CPPFLAGS     += -include ./config.h -include UnitTest++/UnitTest++/UnitTest++.h
AM_CXXFLAGS      = -std=c++14 -ggdb3 -Wall

AM_LDFLAGS       = -static
LDADD            = ../libecfs.la -lboost_serialization -lboost_system -lboost_coroutine -lboost_context -l:libUnitTest++.a

check_PROGRAMS   = settings cache_test messages_test nodes_test #dht_test
#check_SCRIPTS    = integration
check_DATA       = @top_srcdir@/doc/eclipse.json
CLEANFILES       = eclipse.json #integration 

# Input your tests files here
TESTS            = cache_test messages_test nodes_test #dht_test

messages_files   = @top_srcdir@/src/messages/boundaries.cc \
                   @top_srcdir@/src/messages/message.cc    \
                   @top_srcdir@/src/messages/keyvalue.cc   \
                   @top_srcdir@/src/messages/control.cc    \
                   @top_srcdir@/src/messages/boost_impl.cc \
                   @top_srcdir@/src/messages/factory.cc

# Input the dependencies of the test files here
settings_SOURCES      = settings_tests.cc
cache_test_SOURCES    = cache_test.cc
cache_test_LDFLAGS    = -static
nodes_test_SOURCES    = nodes_test.cc \
                        @top_srcdir@/src/nodes/node.cc \
                        @top_srcdir@/src/nodes/nodelocal.cc \
                        @top_srcdir@/src/nodes/peerlocal.cc \
                        @top_srcdir@/src/nodes/noderemote.cc \
                        @top_srcdir@/src/nodes/peerremote.cc \
                        $(messages_files)

messages_test_SOURCES = messages_test.cc $(messages_files)
#dht_test_SOURCES      = dht_test.cc \
#                        @top_srcdir@/src/dht/DHTserver.cc \
#                        @top_srcdir@/src/dht/DHTclient.cc $(messages_files)
