INCLUDES = -I$(top_builddir)/include

sources = bindutils.cpp bindvalues.cpp boundrow.cpp boundvalue.cpp connection.cpp connectionmanager.cpp cursor.cpp error.cpp resultrow.cpp rowcontainer.cpp rowvalue.cpp statement.cpp result.cpp

AM_CPPFLAGS = @MYSQL_CFLAGS@
AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_MYSQL

driver_LTLIBRARIES = tntdb-mysql.la

tntdb_mysql_la_SOURCES = $(sources)
tntdb_mysql_la_LDFLAGS = @MYSQL_LDFLAGS@ -module -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
tntdb_mysql_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
