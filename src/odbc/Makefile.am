AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

sources = \
	connection.cpp \
	connectionmanager.cpp \
	error.cpp \
	handle.cpp \
	statement.cpp

if MAKE_ODBC

driver_LTLIBRARIES = tntdb@abi_current@-odbc.la
tntdb@abi_current@_odbc_la_SOURCES = $(sources)
tntdb@abi_current@_odbc_la_LDFLAGS = -module -version-info @sonumber@ -lodbc
tntdb@abi_current@_odbc_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
