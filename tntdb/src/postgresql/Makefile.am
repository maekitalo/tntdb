INCLUDES = -I$(top_builddir)/include

sources = connection.cpp connectionmanager.cpp cursor.cpp error.cpp result.cpp resultrow.cpp resultvalue.cpp statement.cpp

AM_CPPFLAGS = @PG_CXXFLAGS@

if MAKE_POSTGRESQL_STATIC

lib_LTLIBRARIES = libtntdb-postgresql.la

libtntdb_postgresql_la_SOURCES = $(sources)
libtntdb_postgresql_la_LDFLAGS = @PG_LDFLAGS@ -version-info @sonumber@

else

driver_LTLIBRARIES = tntdb-postgresql.la

tntdb_postgresql_la_SOURCES = $(sources)
tntdb_postgresql_la_LDFLAGS = @PG_LDFLAGS@ -module -version-info @sonumber@
tntdb_postgresql_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
