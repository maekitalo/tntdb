INCLUDES = -I$(top_builddir)/include

sources = bindutils.cpp bindvalues.cpp boundrow.cpp boundvalue.cpp connection.cpp connectionmanager.cpp cursor.cpp error.cpp resultrow.cpp rowcontainer.cpp rowvalue.cpp statement.cpp result.cpp

AM_CPPFLAGS = @MYSQL_CFLAGS@

if MAKE_MYSQL_STATIC

lib_LTLIBRARIES = libtntdb-mysql.la

libtntdb_mysql_la_SOURCES = $(sources)
libtntdb_mysql_la_LDFLAGS = @MYSQL_LDFLAGS@ -version-info @sonumber@

else

driver_LTLIBRARIES = tntdb-mysql.la

tntdb_mysql_la_SOURCES = $(sources)
tntdb_mysql_la_LDFLAGS = @MYSQL_LDFLAGS@ -module -version-info @sonumber@
tntdb_mysql_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
