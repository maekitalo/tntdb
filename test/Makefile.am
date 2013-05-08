INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include

dist_noinst_DATA = \
	mysql-test.sql \
	oracle-test.sql \
	pg-test.sql \
	sqlite-test.sql

noinst_PROGRAMS = \
	tntdb-test

tntdb_test_SOURCES = \
	base-test.cpp \
	colname-test.cpp \
	decimal-test.cpp \
	sqlbuilder-test.cpp \
	test-main.cpp \
	types-test.cpp

AM_LDFLAGS = -lcxxtools-unit
LDADD = $(top_builddir)/src/libtntdb.la
