INCLUDES = -I$(top_srcdir)/include

sources = \
	connection.cpp \
	connectionmanager.cpp \
	cursor.cpp \
	datetime.cpp \
	error.cpp \
	result.cpp \
	row.cpp \
	statement.cpp \
	value.cpp \
	number.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_ORACLE

driver_LTLIBRARIES = tntdb-oracle.la

tntdb_oracle_la_SOURCES = $(sources)
tntdb_oracle_la_LDFLAGS = -module -version-info @sonumber@ @SHARED_LIB_FLAG@ $(PTHREAD_LIBS)
tntdb_oracle_la_LIBADD = $(top_srcdir)/src/libtntdb.la -lclntsh

endif
