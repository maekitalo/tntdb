noinst_PROGRAMS = select modify serial sqlcmd

noinst_HEADERS = \
	util.h

select_SOURCES = select.cpp
modify_SOURCES = modify.cpp
serial_SOURCES = serial.cpp
sqlcmd_SOURCES = sqlcmd.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
LDADD = $(top_builddir)/src/libtntdb.la
