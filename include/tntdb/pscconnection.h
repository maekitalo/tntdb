#ifndef TNTDB_PSCCONNECTION_H
#define TNTDB_PSCCONNECTION_H

#include <tntdb/connection.h>
#include <tntdb/statement.h>
#include <unordered_map>

namespace tntdb
{
/* PSCConnection is short for PreparedStatementCacheConnection.
   It extends a Connection with a prepared statement cache.

   Statements, which are prepared with this connection are stored in a cache.
   When the very same sql statement is prepared again, the stored handle is
   returned. This can speed up queries, which are executed multiple times.

   To use it the used connection class can be replaced with this one.

   @code
    tntdb::PSCConnection conn = tntdb::connect("...");
    auto sel = conn.parepare("select ...");
   @endcode
 */
class PSCConnection : public Connection
{
    typedef std::unordered_map<std::string, Statement> CacheType;
    std::shared_ptr<CacheType> _stmtCache;

public:
    PSCConnection() = default;
    PSCConnection(const PSCConnection&) = default;
    PSCConnection& operator=(const PSCConnection&) = default;

    PSCConnection(tntdb::Connection conn)
        : Connection(conn),
          _stmtCache(std::make_shared<CacheType>())
    { }

    /** Create a new Statement object with the given query and store it in a cache

        When called again with the same query, the cached object is returned
     */
    Statement prepare(const std::string& query);

    /** Create a new Statement object with the given query added by a range limitation.

        This is like `prepareWithLimit` but the prepared statement cache is used.
     */
    Statement prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset = std::string());

    /// Clear the Statement cache used from prepare()
    void clearStatementCache()      { _stmtCache->clear(); }

};
}

#endif
