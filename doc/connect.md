# Connecting to a database

Connections are created with `tntdb::connect()`. The first parameter is a
database URL. It starts with a driver name followed by a colon; the remainder
of the URL is interpreted by the selected driver.

`tntdb::connect()` can also receive a username and password. The tokens `%u`
and `%p` in the URL are replaced with these values before the URL is passed to
the driver. When debug logging is enabled, the URL is logged before this
replacement, so the supplied username and password are not logged as
substituted values.

## Connection cache

`tntdb::connectCached()` behaves like `tntdb::connect()` but uses a connection
cache. On the first call a new connection is created. When the connection
object is released, the connection can be returned to the pool.

A later call to `connectCached()` with the same parameters can reuse the pooled
connection after checking it with a driver-specific ping operation.

If a statement causes an error, the connection is marked dirty and is not
returned to the pool.

The maximum pool size can be controlled using `tntdb::setMaxPoolSize()`. Once
the pool has reached the configured size, released connections are not added to
it.

`tntdb::dropCached()` empties the pool. Connections currently held by the
application are not affected.

## Disconnecting

There is no explicit disconnect function. When the last connection wrapper is
released, the underlying connection is released automatically.

## PostgreSQL driver

The PostgreSQL driver passes the URL after the `postgresql:` prefix to
`PQconnectdb()`.

```cpp
auto conn = tntdb::connect(
    "postgresql:dbname=DS2 user=%u passwd=%p",
    "web",
    "webpw");
```

## MySQL driver

The part after the prefix "mysql:" is split into the parameters needed for
`mysql_real_connect`. The string is interpreted as a list of parameters
separated by semicolon. Each parameter is a name value pair separated by '='.
Valid name keys are 'port', 'flags', 'app', 'host', 'user', 'passwd', 'db' and
'unix socket'. For 'passwd' the word 'password' is also accepted, 'db' may be
replaced with 'dbname' or 'database'. All other keys throw an error. The value
may be enclosed in ' or ". The backslash character ( \\ ) disables any special
meaning of the next character.

```cpp
auto conn = tntdb::connect(
    "mysql:db=DS2;user=%u;passwd=%p",
    "web",
    "webpw");
```

## SQLite driver

The SQLite driver expects a file name as its URL.

```cpp
auto conn = tntdb::connect("sqlite:mydatabase.db");
```

## Oracle driver

The Oracle driver passes the connection string to `OCIServerAttach()`.

```cpp
auto conn = tntdb::connect(
    "oracle:XE;user=%u;passwd=%p",
    "hr",
    "hr");
```

## Replicate driver

Tntdb provides a simple replication driver. Its URL contains two or more
connection strings separated by `|`. Modification statements are executed on
all connections, while selections use the first connection.

See [Replication](replicate.md) for more details.
