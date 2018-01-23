AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

sources = \
	blob.cpp \
	connection.cpp \
	connectionmanager.cpp \
	cursor.cpp \
	datetime.cpp \
	error.cpp \
	multirow.cpp \
	multivalue.cpp \
	number.cpp \
	result.cpp \
	row.cpp \
	singlerow.cpp \
	singlevalue.cpp \
	statement.cpp \
	string.cpp \
	value.cpp

if MAKE_ORACLE

driver_LTLIBRARIES = tntdb@abi_current@-oracle.la

tntdb@abi_current@_oracle_la_SOURCES = $(sources)
tntdb@abi_current@_oracle_la_LDFLAGS = -module -version-info @sonumber@ @SHARED_LIB_FLAG@
tntdb@abi_current@_oracle_la_LIBADD = $(top_builddir)/src/libtntdb.la @ORACLE_OCI_LDFLAGS@ 

endif
