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
	doc/Doxyfile \
	doc/tntdb.odt \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/dirs \
	debian/docs \
	debian/rules \
	debian/shlibs.local \
	debian/tntdb1.dirs \
	debian/tntdb1.install \
	debian/tntdb-dev.dirs \
	debian/tntdb-dev.install \
	debian/tntdb1-pg.dirs \
	debian/tntdb1-pg.install
