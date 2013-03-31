INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include

sources = connection.cpp connectionmanager.cpp statement.cpp

if MAKE_REPLICATE

driver_LTLIBRARIES = tntdb@abi_current@-replicate.la

tntdb@abi_current@_replicate_la_SOURCES = $(sources)
tntdb@abi_current@_replicate_la_LDFLAGS = -module -version-info @sonumber@ @SHARED_LIB_FLAG@
tntdb@abi_current@_replicate_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
