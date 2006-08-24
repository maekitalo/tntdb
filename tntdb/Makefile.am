SUBDIRS =

if MAKE_POSTGRESQL_STATIC
  SUBDIRS += src/postgresql
endif

if MAKE_MYSQL_STATIC
  SUBDIRS += src/mysql
endif

if MAKE_SQLITE_STATIC
  SUBDIRS += src/sqlite
endif

SUBDIRS += src

if MAKE_POSTGRESQL_DYNAMIC
  SUBDIRS += src/postgresql
endif

if MAKE_MYSQL_DYNAMIC
  SUBDIRS += src/mysql
endif

if MAKE_SQLITE_DYNAMIC
  SUBDIRS += src/sqlite
endif

SUBDIRS += \
    include \
    demo

EXTRA_DIST = \
	COPYING.GPL \
	LICENSE.OpenSSL \
	doc/Doxyfile \
	doc/tntdb.odt
