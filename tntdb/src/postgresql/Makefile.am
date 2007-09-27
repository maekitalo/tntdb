INCLUDES = -I$(top_srcdir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp result.cpp resultrow.cpp resultvalue.cpp statement.cpp

AM_CPPFLAGS = @PG_CXXFLAGS@
AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_POSTGRESQL

driver_LTLIBRARIES = tntdb-postgresql.la

tntdb_postgresql_la_SOURCES = $(sources)
tntdb_postgresql_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) @PG_LDFLAGS@ @SHARED_LIB_FLAG@
tntdb_postgresql_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
