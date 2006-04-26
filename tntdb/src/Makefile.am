INCLUDES = -I$(top_builddir)/include

lib_LTLIBRARIES = libtntdb.la

libtntdb_la_SOURCES = \
	connect.cpp \
	connection.cpp \
	connectionpool.cpp \
	date.cpp \
	datetime.cpp \
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
	valueimpl.cpp

libtntdb_la_LDFLAGS = -version-info @sonumber@

if MAKE_POSTGRESQL_STATIC
libtntdb_la_LIBADD = $(top_builddir)/src/postgresql/libtntdb-postgresql.la
endif

if MAKE_MYSQL_STATIC
libtntdb_la_LIBADD = $(top_builddir)/src/mysql/libtntdb-mysql.la
endif

if MAKE_SQLITE_STATIC
libtntdb_la_LIBADD = $(top_builddir)/src/sqlite/libtntdb-sqlite.la
endif
