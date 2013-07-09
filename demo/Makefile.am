noinst_HEADERS = util.h
noinst_PROGRAMS = sqlcmd categories person prodbench pooldemo serial

sqlcmd_SOURCES = sqlcmd.cpp
categories_SOURCES = categories.cpp
person_SOURCES = person.cpp
prodbench_SOURCES = prodbench.cpp
pooldemo_SOURCES = pooldemo.cpp
serial_SOURCES = serial.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
LDADD = $(top_builddir)/src/libtntdb.la
