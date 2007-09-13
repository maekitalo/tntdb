INCLUDES = -I$(top_builddir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp statement.cpp stmtvalue.cpp stmtrow.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_SQLITE

driver_LTLIBRARIES = tntdb-sqlite.la

tntdb_sqlite_la_SOURCES = $(sources)
tntdb_sqlite_la_LDFLAGS = -lsqlite3 -module -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
tntdb_sqlite_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
