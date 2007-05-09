noinst_HEADERS = util.h
noinst_PROGRAMS = sqlcmd categories person prodbench pooldemo

sqlcmd_SOURCES = sqlcmd.cpp
categories_SOURCES = categories.cpp
person_SOURCES = person.cpp
prodbench_SOURCES = prodbench.cpp
pooldemo_SOURCES = pooldemo.cpp

INCLUDES = -I$(top_builddir)/include
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)
LDADD = $(top_builddir)/src/libtntdb.la
