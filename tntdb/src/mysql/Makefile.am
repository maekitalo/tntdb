INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include

sources = bindutils.cpp bindvalues.cpp boundrow.cpp boundvalue.cpp connection.cpp connectionmanager.cpp cursor.cpp error.cpp resultrow.cpp rowcontainer.cpp rowvalue.cpp statement.cpp result.cpp

AM_CPPFLAGS = @MYSQL_CFLAGS@

if MAKE_MYSQL

driver_LTLIBRARIES = tntdb@abi_current@-mysql.la

tntdb@abi_current@_mysql_la_SOURCES = $(sources)
tntdb@abi_current@_mysql_la_LDFLAGS = -module -version-info @sonumber@ @MYSQL_LDFLAGS@ @SHARED_LIB_FLAG@
tntdb@abi_current@_mysql_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
