AC_INIT(tntdb, 1.4, [Tommi Maekitalo <tommi@tntnet.org>])
AM_INIT_AUTOMAKE
LT_INIT([disable-static])

abi_current=5
abi_revision=0
abi_age=0
sonumber=${abi_current}:${abi_revision}:${abi_age}
AC_SUBST(sonumber)
AC_SUBST(abi_current)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([include/tntdb/config.h])
AC_CONFIG_SRCDIR([src/connect.cpp])

AC_PROG_CXX
AC_LANG(C++)
AX_CXX_COMPILE_STDCXX_11(noext, optional)

ACX_PTHREAD

CC="$PTHREAD_CC"
CXX="$PTHREAD_CXX"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CXXFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"

AX_COMPILER_VENDOR
AS_IF([test "$ax_cv_cxx_compiler_vendor" = "ibm"],
      [CPPFLAGS="$CPPFLAGS -qrtti -qlanglvl=newexcp -D__NOLOCK_ON_INPUT -D__NOLOCK_ON_OUTPUT"],
      AX_CHECK_COMPILE_FLAG([-Wno-long-long], [CPPFLAGS="$CPPFLAGS -Wno-long-long"])
      AX_CHECK_COMPILE_FLAG([-Wall], [CPPFLAGS="$CPPFLAGS -Wall"])
      AX_CHECK_COMPILE_FLAG([-pedantic], [CPPFLAGS="$CPPFLAGS -pedantic"]))

AC_MSG_CHECKING([for sighandler_t])
AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    #include <signal.h>
    sighandler_t *f;])],
  has_sighandler_t=yes,has_sighandler_t=no)

AC_MSG_RESULT($has_sighandler_t)
AS_IF([test "$has_sighandler_t" = "yes"],
    [AC_DEFINE( HAVE_SIGHANDLER_T, 1, [Define if sighandler_t available] )])

#
# check standard integer types
#
AC_TYPE_LONG_LONG_INT
AS_IF(
    [test "$ac_cv_type_long_long_int" = yes],
    [HAVE_LONG_LONG=HAVE_LONG_LONG],
    [HAVE_LONG_LONG=NO_LONG_LONG])

AC_SUBST(HAVE_LONG_LONG, "$HAVE_LONG_LONG")

AC_TYPE_UNSIGNED_LONG_LONG_INT
AS_IF(
    [test "$ac_cv_type_unsigned_long_long_int" = yes],
    [HAVE_UNSIGNED_LONG_LONG=HAVE_UNSIGNED_LONG_LONG],
    [HAVE_UNSIGNED_LONG_LONG=NO_UNSIGNED_LONG_LONG])

AC_SUBST(HAVE_UNSIGNED_LONG_LONG, "$HAVE_UNSIGNED_LONG_LONG")

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    #include <stdint.h>
    void foo(int) {}
    void foo(long) {}
    void foo(int32_t) {}
    ])],
INT_INT32_T_CONFLICT=0,
INT_INT32_T_CONFLICT=1)
AC_SUBST(INT_INT32_T_CONFLICT)

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    #include <stdint.h>
    void foo(unsigned) {}
    void foo(unsigned long) {}
    void foo(uint32_t) {}
    ])],
UNSIGNED_UINT32_T_CONFLICT=0,
UNSIGNED_UINT32_T_CONFLICT=1)
AC_SUBST(UNSIGNED_UINT32_T_CONFLICT)

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    #include <stdint.h>
    void foo(int) {}
    void foo(long) {}
    void foo(int64_t) {}
    ])],
INT_INT64_T_CONFLICT=0,
INT_INT64_T_CONFLICT=1)
AC_SUBST(INT_INT64_T_CONFLICT)

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    #include <stdint.h>
    void foo(unsigned) {}
    void foo(unsigned long) {}
    void foo(uint64_t) {}
    ])],
UNSIGNED_UINT64_T_CONFLICT=0,
UNSIGNED_UINT64_T_CONFLICT=1)
AC_SUBST(UNSIGNED_UINT64_T_CONFLICT)

#
# Cxxtools
#
AC_CHECK_HEADER([cxxtools/condition.h], , AC_MSG_ERROR([cxxtools headers not found]))
LDFLAGS="$LDFLAGS -lcxxtools"

#
# Postgresql
#
AC_ARG_WITH(
  [postgresql],
  AS_HELP_STRING([--with-postgresql],
                 [build postgresql driver (default: yes)]),
  [with_postgresql=$withval],
  [with_postgresql=yes])

AS_IF([test "$with_postgresql" = yes],
[
  AC_CHECK_PROG(PG_CONFIG, [pg_config], [pg_config])
  if test -z "$PG_CONFIG"; then
    AC_MSG_ERROR([postgresql configuration script pg_config was not found; install postgresql developent package or use --without-postgresql])
  fi

  PG_CPPFLAGS=-I`$PG_CONFIG --includedir`
  PG_LDFLAGS="-L`$PG_CONFIG --libdir` -lpq"
  AC_SUBST(PG_CONFIG)
  AC_SUBST(PG_CPPFLAGS)
  AC_SUBST(PG_LDFLAGS)

  save_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $PG_CPPFLAGS"

  AC_CHECK_HEADER([libpq-fe.h], , AC_MSG_ERROR([postgresql headers not found]))

  AC_MSG_CHECKING([for PQprepare in libpq-fe.h])

  AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([#include <libpq-fe.h>
      void t() { PQprepare(0, 0, 0, 0, 0); }])],
    [AC_DEFINE(HAVE_PQPREPARE, 1, [Defined if postgresql supports PQprepare])
     AC_MSG_RESULT(yes)
    ],
     AC_MSG_RESULT(no))
  CPPFLAGS=$save_CPPFLAGS
])

AM_CONDITIONAL(MAKE_POSTGRESQL, test "$with_postgresql" = yes)

#
# mysql
#
AC_ARG_WITH(
  [mysql],
  AS_HELP_STRING([--with-mysql],
                 [build mysql driver (default: yes)]),
  [ with_mysql=$withval ],
  [ with_mysql=yes ])

AS_IF([test "$with_mysql" = yes],
[
  AC_CHECK_PROG(MYSQL_CONFIG, mysql_config, mysql_config)
  if test -z "$MYSQL_CONFIG"; then
    AC_MSG_ERROR([mysql configuration script was not found; install mysql developent package or use --without-mysql])
  fi

  MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
  MYSQL_LDFLAGS=`$MYSQL_CONFIG --libs_r`

  AC_SUBST(MYSQL_CONFIG)
  AC_SUBST(MYSQL_CFLAGS)
  AC_SUBST(MYSQL_LDFLAGS)
])

AM_CONDITIONAL(MAKE_MYSQL, test "$with_mysql" = yes)

#
# Sqlite
#
AC_ARG_WITH(
  [sqlite],
  AS_HELP_STRING([--with-sqlite],
                 [build sqlite driver (default: yes)]),
  [with_sqlite=$withval],
  [with_sqlite=yes])

AS_IF([test "$with_sqlite" = yes],
[
  AC_CHECK_HEADER([sqlite3.h], , AC_MSG_ERROR([header for sqlite3 not found; install sqlite developent package or use --without-sqlite]))

  # check for sqlite3_transfer_bindings
  AC_MSG_CHECKING([for sqlite3_transfer_bindings])
  AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([#include <sqlite3.h>
      void t() { sqlite3_transfer_bindings(0, 0); }])],
    [AC_MSG_RESULT(yes)],
    [AC_MSG_ERROR([sqlite3_transfer_bindings is missing])])

  # check for sqlite3_prepare_v2
  AC_MSG_CHECKING([for sqlite3_prepare_v2])
  AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([#include <sqlite3.h>
      void t() { sqlite3_prepare_v2(0, 0, 0, 0, 0); }])],
    [AC_DEFINE(HAVE_SQLITE3_PREPARE_V2, 1, [Defined if sqlite have sqlite3_prepare_v2])
     AC_MSG_RESULT(yes)
    ],
     AC_MSG_RESULT(no))
])

AM_CONDITIONAL(MAKE_SQLITE, test "$with_sqlite" = yes)

#
# Oracle
#
AC_ARG_WITH(
  [oracle],
  AS_HELP_STRING([--with-oracle],
                 [build oracle-driver (default: no)]),
  [with_oracle=$withval],
  [with_oracle=no])

if test "$with_oracle" = yes
then
  AC_CHECK_HEADER([oci.h], , AC_MSG_ERROR([header for oracle not found]))
  ORACLE_OCI_LDFLAGS=-lclntsh
  AC_SUBST(ORACLE_OCI_LDFLAGS)
fi

AM_CONDITIONAL(MAKE_ORACLE, test "$with_oracle" = yes)

#
# Replicate
#
AC_ARG_WITH(
  [replicate],
  AS_HELP_STRING([--with-replicate],
                 [build replicate driver (default: yes)]),
  [with_replicate=$withval],
  [with_replicate=yes])

AM_CONDITIONAL(MAKE_REPLICATE, test "$with_replicate" = yes)

#
# Doxygen-documentation
#
AC_ARG_VAR(DOXYGEN, [doxygen documentation generator])
AC_ARG_WITH(
  [doxygen],
  AS_HELP_STRING([--with-doxygen],
                 [generate doxygen documentation]),
  [doxygenFlag=$withval],
  [doxygenFlag=no])

AS_IF([test "$doxygenFlag" != "no"],
[
    AC_CHECK_PROG(DOXYGEN, doxygen, doxygen)
    if test "$DOXYGEN" == ""
      then
        AC_MSG_ERROR([doxygen not found])
      fi
    AC_DEFINE(WITH_DOXYGEN, 1, [build doxygen documentation])
])

AM_CONDITIONAL(MAKE_DOXYGEN, test "$doxygenFlag" != "no")
AC_SUBST(DOXYGEN)

# driverlibdir
AC_ARG_WITH(
  [driverdir],
  AS_HELP_STRING([--with-driverdir=dir],
                 [specify directory for driver files [[LIBDIR/tntdb]] ]),
  [driverdir=$withval],
  [driverdir=$libdir/tntdb])
AC_SUBST(driverdir)

AS_IF([test "$driverdir" == "yes"],
    [driverdir=$libdir/tntdb])

# additional shared library checks
case "${host_cpu}-${host_os}" in
*-aix*)
    SHARED_LIB_FLAG=-qmkshrobj
    ;;
*)
    SHARED_LIB_FLAG=
    ;;
esac
AC_SUBST(SHARED_LIB_FLAG)

AC_CONFIG_FILES([
  Makefile
  doc/Makefile
  doc/tntdb.doxygen
  demo/Makefile
  include/Makefile
  pkgconfig/tntdb.pc
  src/Makefile
  src/mysql/Makefile
  src/postgresql/Makefile
  src/replicate/Makefile
  src/sqlite/Makefile
  src/oracle/Makefile
  test/Makefile
  ])

AC_OUTPUT
