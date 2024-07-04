#include <tntdb/pscconnection.h>
#include <cxxtools/log.h>

log_define("tntdb.PSCConnection")

namespace tntdb
{
Statement PSCConnection::prepare(const std::string& query)
{
    log_debug("prepare(\"" << query << "\")");
    auto it = _stmtCache->find(query);
    if (it != _stmtCache->end())
    {
        log_debug("query found in cache");
        return it->second;
    }
    log_debug("query not found in cache");
    auto stmt = Connection::prepare(query);
    _stmtCache->emplace(query, stmt);
    return stmt;
}

Statement PSCConnection::prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset)
{
    log_debug("prepareWithLimit(\"" << query << "\", " << limit << ", " << offset << ')');
    auto key = query + '\0' + limit + '\0' + offset;
    auto it = _stmtCache->find(key);
    if (it != _stmtCache->end())
    {
        log_debug("query found in cache");
        return it->second;
    }

    log_debug("query not found in cache");
    auto stmt = Connection::prepareWithLimit(query, limit, offset);
    _stmtCache->emplace(key, stmt);
    return stmt;
}

}
