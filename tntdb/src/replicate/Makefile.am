INCLUDES = -I$(top_srcdir)/include

sources = connection.cpp connectionmanager.cpp statement.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_REPLICATE

driver_LTLIBRARIES = tntdb@abi_current@-replicate.la

tntdb@abi_current@_replicate_la_SOURCES = $(sources)
tntdb@abi_current@_replicate_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
tntdb@abi_current@_replicate_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
