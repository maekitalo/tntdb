INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include

noinst_PROGRAMS = \
	tntdb-test

tntdb_test_SOURCES = \
	colname-test.cpp \
	decimal-test.cpp \
	sqlbuilder-test.cpp \
	test-main.cpp \
	types-test.cpp

AM_LDFLAGS = -lcxxtools-unit
LDADD = $(top_builddir)/src/libtntdb.la
