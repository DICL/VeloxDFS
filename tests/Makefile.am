AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include unittest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a -ldl -lsqlite3

# Input your tests files here
check_PROGRAMS   = units metadata 
check_SCRIPTS    = integration

TESTS            = units 

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
                     src/network/asyncchannel.cc \
                     src/network/server.cc \
                     src/nodes/machine.cc \
                     src/nodes/node.cc 

metadata_LDADD     = $(LDADD_T)
metadata_CPPFLAGS  = $(AM_CPPFLAGS_T)
metadata_SOURCES   = tests/metadata_test.cc \
                     src/fileleader/directory.cc \
                     $(messages_files)

check-integration: integration
	bash integration.sh

integration:
	cp $(top_srcdir)/tests/integration_test.sh integration.sh
	chmod +x integration.sh
