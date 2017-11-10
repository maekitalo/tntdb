Format: 1.0
Source: tntdb
Binary: libtntdb4, tntdb-mysql4, tntdb-postgresql4, tntdb-sqlite4, libtntdb-dev, tntdb-dbg
Architecture: any
Version: 1.3-2etn2
Maintainer: Kari Pahula <kaol@debian.org>
Homepage: http://www.tntnet.org/tntdb.html
Standards-Version: 3.9.4
Build-Depends: cdbs, debhelper (>= 9), libcxxtools-dev (>= 2.2), libsqlite3-dev, libmariadbclient-dev, libpq-dev, libltdl-dev, doxygen, dh-autoreconf, sqlite3
Build-Conflicts: libtntdb-dev, libtntdb3
Package-List: 
 libtntdb-dev deb libdevel optional
 libtntdb4 deb libs optional
 tntdb-mysql4 deb libs optional
 tntdb-postgresql4 deb libs optional
 tntdb-sqlite4 deb libs optional
 tntdb-dbg deb debug extra
DEBTRANSFORM-TAR: tntdb-1.3.tar.gz
