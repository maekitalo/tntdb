/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNTDB_BITS_CONNECTION_H
#define TNTDB_BITS_CONNECTION_H

#include <string>
#include <tntdb/smartptr.h>
#include <tntdb/iface/iconnection.h>
#include <tntdb/bits/statement.h>

namespace tntdb
{
  class Result;
  class Row;
  class Value;
  class Statement;

  /**
   * This class holds a connection to a database.
   *
   * Normally you will create a connection with tntdb::connect(url). The actual
   * connection is referencecounted. You can copy this class as you need. When
   * the last copy of it is destroyed, the connection is closed.
   *
   * Example:
   * \code
   *   try
   *   {
   *     tntdb::Connection conn = tntdb::connect("postgresql:dbname=mydatabase");
   *     tntdb::Result res = conn.select("select col1, col2 from mytable");
   *     for (tntdb::Result::const_iterator it = res.begin(); it != res.end(); ++it)
   *       std::cout << "col1=" << it->getString() << " col2=" << it->getInt() << std::endl;
   *   }
   *   catch (const std::exception& e)
   *   {
   *     std::cerr << e.what() << std::endl;
   *   }
   * \endcode
   */
  class Connection
  {
    public:
      typedef unsigned size_type;

    private:
      SmartPtr<IConnection, InternalRefCounted> conn;

    public:
      /**
       * Instantiate a empty connection-object.
       */
      Connection() { }
      /**
       * Initialize this class with a connection.
       */
      Connection(IConnection* conn_)
        : conn(conn_)
        { }

      /**
       * Remove the reference to the connected database. If this was the last
       * reference, the connection is actually closed.
       */
      void close()
        { conn = 0; }

      /**
       * Starts a transaction. Normally this is not needed. It is better to use
       * the class tntdb::Transaction instead.
       */
      void beginTransaction();
      /**
       * Commits the current transaction.
       */
      void commitTransaction();
      /**
       * Rolls back the current transaction.
       */
      void rollbackTransaction();

      /**
       * Executes a static query, without returning results. The query is
       * normally a INSERT-, UPDATE- or DELETE-statement. As with the other
       * query-execution-methods this should be used only for static queries.
       * When you need to pass parameters it is always better to use
       * tntdb::Statement.
       */
      size_type execute(const std::string& query);
      /**
       * Executes a static query, which returns a result. The query is normally
       * a SELECT-statement.
       */
      Result select(const std::string& query);
      /**
       * Executes a static query, which returns a result. The first row is
       * returned. If the query does not return rows, the exception
       * tntdb::NotFound is thrown.
       */
      Row selectRow(const std::string& query);
      /**
       * Executes a static query, which returns a result. The first value of
       * the first row is returned. If the query does not return rows, the
       * exception tntdb::NotFound is thrown.
       */
      Value selectValue(const std::string& query);
      /**
       * Creates a new Statement-object, with the given query.
       */
      Statement prepare(const std::string& query);
      /**
       * Creates a new Statement-object, with the given query
       * and stores the statement in a cache. When called again
       * with the same query, the cached result is returned.
       */
      Statement prepareCached(const std::string& query);

      /**
       * Clears the cache, built with prepareCache.
       */
      void clearStatementCache()         { conn->clearStatementCache(); }

      /**
       * Returns true, when there is no connection established.
       */
      bool operator!() const             { return !conn; }
      /**
       * Returns the actual implementation-class.
       */
      const IConnection* getImpl() const { return &*conn; }
  };
}

#endif // TNTDB_BITS_CONNECTION_H

