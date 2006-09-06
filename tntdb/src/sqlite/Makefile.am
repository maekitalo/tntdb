INCLUDES = -I$(top_builddir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp statement.cpp stmtvalue.cpp stmtrow.cpp

if MAKE_SQLITE_STATIC

lib_LTLIBRARIES = libtntdb-sqlite.la

libtntdb_sqlite_la_SOURCES = $(sources)
libtntdb_sqlite_la_LDFLAGS = -lsqlite3 -module -version-info @sonumber@

else

pkglib_LTLIBRARIES = tntdb-sqlite.la

tntdb_sqlite_la_SOURCES = $(sources)
tntdb_sqlite_la_LDFLAGS = -lsqlite3 -module -version-info @sonumber@

endif
