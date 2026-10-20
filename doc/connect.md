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

The MySQL driver uses `mysql_real_connect()`. Tntdb parses the URL as
name/value pairs separated by semicolons, with `=` separating each name from
its value.

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

```cpp
auto conn = tntdb::connect(
    "replicate:postgresql:dbname=DS2 user=%u passwd=%p|sqlite:mydatabase.db",
    "web",
    "webpw");
```

If username and password placeholders are used, the same credentials are supplied to all peers.
