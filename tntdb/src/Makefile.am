INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libtntdb.la

libtntdb_la_SOURCES = \
	blob.cpp \
	blobstream.cpp \
	connect.cpp \
	connection.cpp \
	connectionpool.cpp \
	date.cpp \
	datetime.cpp \
	decimal.cpp \
	error.cpp \
	librarymanager.cpp \
	poolconnection.cpp \
	result.cpp \
	resultimpl.cpp \
	row.cpp \
	rowimpl.cpp \
	statement.cpp \
	statement_iterator.cpp \
	stmtparser.cpp \
	time.cpp \
	transaction.cpp \
	valueimpl.cpp

libtntdb_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
libtntdb_la_CXXFLAGS = -DDRIVERDIR=\"@driverdir@\" $(PTHREAD_CFLAGS)
