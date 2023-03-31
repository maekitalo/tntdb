/*
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef TNTDB_BITS_CONNECTION_H
#define TNTDB_BITS_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <tntdb/bits/statement.h>
#include <string>
#include <memory>

#if __cplusplus >= 201403L
#define DEPRECATED [[deprecated]]
#else
#define DEPRECATED
#endif

namespace tntdb
{
class Result;
class Row;
class Value;
class Statement;

/** This class holds a connection to a database

    Normally you would create a connection with tntdb::connect(url). The actual
    connection is reference counted. You can copy this class as you need. When
    the last copy of it is destroyed, the connection is closed.

    Example:
    @code
      try
      {
        tntdb::Connection conn = tntdb::connect("postgresql:dbname=mydatabase");
        tntdb::Statement st = conn.prepare("SELECT col1, col2 FROM mytable");

        for (tntdb::Statement::const_iterator it = res.begin(); it != res.end(); ++it)
          std::cout << "col1 = " << it->getString() << ", col2 = " << it->getInt() << std::endl;
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    @endcode
 */
class Connection
{
public:
    typedef unsigned size_type;

private:
    std::shared_ptr<IConnection> _conn;

public:
    /// Create an empty %Connection object
    Connection() { }

    /// Create a %Connection object from conn
    explicit Connection(const std::shared_ptr<IConnection>& conn)
      : _conn(conn)
      { }

    /** Remove the reference to the connected database. If this was
        the last reference, the connection is actually closed.
     */
    void close() { _conn = 0; }

    /** Start a transaction

        Normally this is not needed. It is better to use the class Transaction instead.
     */
    void beginTransaction();

    /// Commit the current transaction
    void commitTransaction();

    /// Roll back the current transaction
    void rollbackTransaction();

    /** Execute a static query without returning results

        The query normally is an INSERT, UPDATE or DELETE statement. As with the
        other query execution methods this should be used only for static queries.
        If you need to pass parameters it is always better to use the Statement class.
     */
    size_type execute(const std::string& query);

    /** Execute a static query which returns a result

        The query normally is a SELECT statement.
     */
    Result select(const std::string& query);

    /** Execute a static query which returns a result

        The first row is returned. If the query returns an empty
        result, a NotFound exception is thrown.
     */
    Row selectRow(const std::string& query);

    /** Execute a static query which returns a result

        The first value of the first row is returned. If the query
        returns an empty result, a NotFound exception is thrown.
     */
    Value selectValue(const std::string& query);

    /// Create a new Statement object with the given query
    Statement prepare(const std::string& query);

    /** Create a new Statement object with the given query added by a range limitation.

        The `limit` and `offset` parameters are the names of the host variables, which receive the limits.
        The offset is optional. When an empty string is passed to offset, just the limit is used.
     */
    Statement prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset = std::string());

    /** Create a new Statement object with the given query and store it in a cache

        When called again with the same query, the cached object is returned
     */
    DEPRECATED
    Statement prepareCached(const std::string& query)
      { return prepare(query); }

    /** Create a new Statement object with the given query and caching key

        Per default, prepareCached() uses the query as key which identifies
        a Statement object in the cache. With this method you can provide
        your own key (because shorter keys mean faster lookup in the cache).
        Be aware though that when using this, you have to ensure the same
        key won't be used twice.
     */
    DEPRECATED
    Statement prepareCached(const std::string& query, const std::string&)
      { return prepare(query); }

    /** Create a new Statement object with the given query added by a range limitation.

        This is like `prepareWithLimit` but the prepared statement cache is used.
     */
    DEPRECATED
    Statement prepareCachedWithLimit(const std::string& query, const std::string& limit, const std::string& offset = std::string())
      { return prepareWithLimit(query, limit, offset); }

    /** Create a new Statement object with the given query added by a range limitation.

        This is like `prepareWithLimit` but the prepared statement cache is
        used. Like in `prepareCached` an optional key can be used.
     */
    DEPRECATED
    Statement prepareCachedWithLimit(const std::string& query, const
    std::string& limit, const std::string& offset, const std::string&)
      { return prepareWithLimit(query, limit, offset); }

    /// Clear the Statement cache used from prepareCached()
    DEPRECATED
    void clearStatementCache()
      { }

    /** Remove a query from the statement cache

        The return value is true if the given key was found in the cache, false otherwise.
     */
    DEPRECATED
    bool clearStatementCache(const std::string&)
      { return true; }

    /// Check whether the connection is alive
    bool ping()                        { return _conn->ping(); }

    /// Get the last inserted insert id
    long lastInsertId(const std::string& name = std::string())
      { return _conn->lastInsertId(name); }

    /// Check if a connection is established (<b>true if not</b>)
    bool operator!() const             { return !_conn; }

    /// @{
    /// Get the actual implementation object
    std::shared_ptr<IConnection> getImpl() const { return _conn; }
    /// @}
};
}

#endif // TNTDB_BITS_CONNECTION_H

