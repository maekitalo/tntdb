# Connecting to a database

Connecting to a database is done using the function `tntdb::connect`. It
expects either one or three parameters. The first parameter is the url. The url
always starts with a driver name and a colon. The rest is passed to the driver
and hence specific to it.

When the optional second and third parameters are the username and the
password. Those can be used int the database url. The token '%u' is replaced by
the username and the token '%p' is replaced by the password before passed to
the driver. Note that the url is logged before the repacement when debug log is
enabled but the username and password is not.

## Connection cache

Tntdb maintains a connection cache. The function `tntdb::connectCached` works
like `tntdb::connect` but uses a connection cache. On the first call it does
the action connect. As soon as the connection object is released, the connection
is put into the pool. When `tntdb::connectCached` is called then with the exact
same parameters, it removes the connection from the pool and returns it. But
before returning the connection it checks the validity by issuing a ping
command, which is database specific.

Also when a statement issues an error in some point, the connection is marked
as dirty and not put back into the pool for safety. An error is a hint, that
there may be something wrong.

The cache can also be limited to a maximum size using `tntdb::setMaxPoolSize`.
When the pool has reached its maximum size, connections, which are released
are not put back to the pool.

The function `tntdb::dropCached` can be used to empty the pool if needed. Note
that this does not affect connections, which are held by the application.

## Disconnecting

There is no specific disconnect. When the database connection goes out of
scope, the connection is also disconnected.

## Postgresql driver

The PostgreSQL driver passes the url without the prefix directly to the driver
function PQconnectdb. See the PostgreSQL documentation for details.

### Example:

```cpp
    auto conn = tntdb::connect("postgresql:dbname=DS2 user=%u passwd=%p", "web", "webpw");
```

## MySql driver

The MySql driver uses the function `m̀ysql\_real\_connect`. The function expects the
connection parameters as separate entities. Tntdb parses for that the url to get those.
The url is a list of name value pairs separated by semicolon. The name and value is
separated by '='.

### Example:

```cpp
    auto conn = tntdb::connect("mysql:db=DS2;user=%u;passwd=%p", "web", "webpw");
```

## Sqlite3 driver

The Sqlite3 driver expects just a single file name as url.

### Example:

```cpp
    auto conn = tntdb::connect("sqlite:mydatabase.db");
```

## Oracle driver

The Oracle driver passes the connection string to the function OCIServerAttach.

### Example:

```cpp
    auto conn = tntdb::connect("oracle:XE;user=%u;passwd=%p", "hr", "hr");
```

## Replicate

A very simple replication driver is offered by Tntdb.
The connection string expects 2 or more connection strings separated by '|'.
When SQL statements are executed, they are executed on all connections.
Selecting data is done only on the first connection.

### Example:

```cpp
    auto conn = tntdb::connect("replicate:postgresql:dbname=DS2 user=%u passwd=%p|sqlite:mydatabase.db", "web", "webpw");
```

Note that if username and password is used, they must be identical in all peers.
