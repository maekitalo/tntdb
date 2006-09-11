INCLUDES = -I$(top_builddir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp statement.cpp stmtvalue.cpp stmtrow.cpp

if MAKE_SQLITE_STATIC

lib_LTLIBRARIES = libtntdb-sqlite.la

libtntdb_sqlite_la_SOURCES = $(sources)
libtntdb_sqlite_la_LDFLAGS = -lsqlite3 -module -version-info @sonumber@

else

driver_LTLIBRARIES = tntdb-sqlite.la

tntdb_sqlite_la_SOURCES = $(sources)
tntdb_sqlite_la_LDFLAGS = -lsqlite3 -module -version-info @sonumber@
tntdb_sqlite_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
