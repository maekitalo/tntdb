INCLUDES = -I$(top_srcdir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp statement.cpp stmtvalue.cpp stmtrow.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_SQLITE

driver_LTLIBRARIES = tntdb2-sqlite.la

tntdb2_sqlite_la_SOURCES = $(sources)
tntdb2_sqlite_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) -lsqlite3 @SHARED_LIB_FLAG@
tntdb2_sqlite_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
