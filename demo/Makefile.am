noinst_PROGRAMS = select modify serial sqlcmd csvoutput serialize deserialize

noinst_HEADERS = \
	util.h

select_SOURCES = select.cpp
modify_SOURCES = modify.cpp
serial_SOURCES = serial.cpp
sqlcmd_SOURCES = sqlcmd.cpp
csvoutput_SOURCES = csvoutput.cpp
serialize_SOURCES = serialize.cpp
deserialize_SOURCES = deserialize.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
LDADD = $(top_builddir)/src/libtntdb.la
