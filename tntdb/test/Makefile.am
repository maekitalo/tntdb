noinst_PROGRAMS = \
    tntdb-test

tntdb_test_SOURCES = test-main.cpp types-test.cpp decimal-test.cpp

AM_CPPFLAGS = -I$(top_builddir)/include

AM_LDFLAGS = -lcxxtools-unit
LDADD = $(top_srcdir)/src/libtntdb.la
