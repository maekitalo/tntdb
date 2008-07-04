INCLUDES = -I$(top_srcdir)/include

sources = \
	blob.cpp \
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

driver_LTLIBRARIES = tntdb1-oracle.la

tntdb1_oracle_la_SOURCES = $(sources)
tntdb1_oracle_la_LDFLAGS = -module -version-info @sonumber@ $(PTHREAD_LIBS) -lclntsh @SHARED_LIB_FLAG@
tntdb1_oracle_la_LIBADD = $(top_srcdir)/src/libtntdb.la

endif
