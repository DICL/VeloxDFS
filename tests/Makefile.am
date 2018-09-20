AM_CPPFLAGS_T    = $(AM_CPPFLAGS) -include unittest++/UnitTest++.h
LDADD_T          = $(LDADD) -l:libUnitTest++.a

check_DATA       = $(top_srcdir)/doc/eclipse.json
CLEANFILES       = eclipse.json
check_PROGRAMS   = 
TESTS            = 

#<<<<<<<<<<<<<<<<Input your tests files here>>>>>>>>>>>>>>>>>>>>

# INTEGRATION TEST
# ---------------------------------------------------------------------
check_SCRIPTS     = integration 
check-integration: integration
	bash integration.sh

integration:
	cp $(top_srcdir)/tests/integration_test.sh integration.sh
	chmod +x integration.sh


# MESSAGES TEST
# ---------------------------------------------------------------------
check_PROGRAMS    += messages
#TESTS             += messages
messages_LDADD     = $(LDADD_T) -ldl -lsqlite3
messages_CPPFLAGS  = $(AM_CPPFLAGS_T)
messages_SOURCES   = $(messages_files) \
                     tests/main.cc \
                     tests/messages_test.cc \
                     src/network/asyncchannel.cc \
                     src/network/server.cc \
                     src/nodes/machine.cc \
                     src/nodes/node.cc

# METADATA TEST
# ---------------------------------------------------------------------
check_PROGRAMS    += metadata 
metadata_LDADD     = $(LDADD_T) -ldl -lsqlite3  
metadata_CPPFLAGS  = $(AM_CPPFLAGS_T)
metadata_SOURCES   = tests/metadata_test.cc \
                     src/fileleader/directory.cc \
                     $(messages_files)

# LOGICAL BLOCKS TEST
# ---------------------------------------------------------------------
if LOGICAL_BLOCKS_FEATURE
check_PROGRAMS   += lblocks
TESTS            += lblocks

lblocks_LDADD     = $(LDADD_T) -lzookeeper_mt
lblocks_CPPFLAGS  = $(AM_CPPFLAGS_T)
lblocks_SOURCES   = tests/main.cc \
                    tests/stats_scheduler_test.cc \
                    src/stats/scheduler_factory.cc \
                    src/stats/scheduler_score_based.cc \
                    src/stats/scheduler_simple.cc \
                    src/stats/scheduler_python.cc \
                    src/stats/scheduler_vlmb.cc \
                    src/stats/scheduler_multiwave.cc \
                    src/stats/zk_listener.cc
endif
