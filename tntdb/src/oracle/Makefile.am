INCLUDES = -I$(top_builddir)/include

sources = \
	connection.cpp \
	connectionmanager.cpp \
	cursor.cpp \
	datetime.cpp \
	error.cpp \
	result.cpp \
	row.cpp \
	statement.cpp \
	value.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)

if MAKE_ORACLE

driver_LTLIBRARIES = tntdb-oracle.la

tntdb_oracle_la_SOURCES = $(sources)
tntdb_oracle_la_LDFLAGS = -lclntsh -module -version-info @sonumber@ $(PTHREAD_LIBS)
tntdb_oracle_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
