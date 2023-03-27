AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

dist_noinst_DATA = \
	mysql-test.sql \
	oracle-test.sql \
	pg-test.sql \
	sqlite-test.sql

noinst_PROGRAMS = \
	tntdb-test

tntdb_test_SOURCES = \
	testbase.cpp \
	base-test.cpp \
	bin-test.cpp \
	colname-test.cpp \
	decimal-test.cpp \
	json-test.cpp \
	sqlbuilder-test.cpp \
	statement-test.cpp \
	test-main.cpp \
	timespan-test.cpp \
	types-test.cpp \
	value-test.cpp

AM_LDFLAGS = -lcxxtools-unit -lcxxtools-bin
LDADD = $(top_builddir)/src/libtntdb.la
