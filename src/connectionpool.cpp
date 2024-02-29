/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#include <tntdb/connectionpool.h>
#include <tntdb/connect.h>
#include <tntdb/impl/poolconnection.h>
#include <cxxtools/log.h>

log_define("tntdb.connectionpool")

namespace tntdb
{
////////////////////////////////////////////////////////////////////////
// ConnectionPool
//
Connection ConnectionPool::connect()
{
    log_debug("ConnectionPool::connect()");

    log_debug("current pool size " << getCurrentSize() << " max " << getMaxSpare());

    std::lock_guard<std::mutex> lock(_mutex);

    std::shared_ptr<IConnection> c;
    while (!_connectionPool.empty()
        && !_connectionPool.back()->ping())
    {
        log_warn("drop dead connection from pool");
        _connectionPool.pop_back();
    }

    if (_connectionPool.empty())
    {
        c = tntdb::connect(_url, _username, _password).getImpl();
    }
    else
    {
        c = std::move(_connectionPool.back());
        _connectionPool.pop_back();
    }
    
    return Connection(std::make_shared<PoolConnection>(std::move(c), *this));
}

void ConnectionPool::put(std::shared_ptr<IConnection>& conn)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_maxSpare == 0 || _connectionPool.size() < _maxSpare)
        _connectionPool.emplace_back(conn);
    else
        log_debug("don't reuse connection " << conn << " max spare " << _maxSpare << " reached");
}

void ConnectionPool::drop(unsigned keep)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_connectionPool.size() > keep)
        _connectionPool.resize(keep);
}

void ConnectionPool::setMaxSpare(unsigned m)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _maxSpare = m;
    if (_connectionPool.size() > _maxSpare)
        _connectionPool.resize(_maxSpare);
}

////////////////////////////////////////////////////////////////////////
// ConnectionPools
//
Connection ConnectionPools::connect(const std::string& url, const std::string& username, const std::string& password)
{
    log_debug("ConnectionPools::connect(\"" << url << "\", \"" << username << "\", password)");

    PoolsType::iterator it;

    {
        std::lock_guard<std::mutex> lock(_mutex);
        it = _pools.find(ConnectionParameter(url, username, password));
        if (it == _pools.end())
        {
            log_debug("create pool for url \"" << url << "\" user \"" << username << "\" with " << _maxcount << " connections");
            PoolType* pool = new PoolType(url, username, password, _maxcount);
            it = _pools.emplace(ConnectionParameter(url, username, password), pool).first;
        }
        else
            log_debug("pool for url \"" << url << "\" found");
    }

    log_debug("current pool size " << it->second->getCurrentSize());
    return it->second->connect();
}

void ConnectionPools::drop(unsigned keep)
{
    log_debug("drop(" << keep << ')');

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& p: _pools)
        p.second->drop(keep);
}

void ConnectionPools::drop(const std::string& url, const std::string& username, const std::string& password, unsigned keep)
{
    log_debug("drop(\"" << url << "\", \"" << username << "\", password, " << keep << ')');

    std::lock_guard<std::mutex> lock(_mutex);

    PoolsType::iterator it = _pools.find(ConnectionParameter(url, username, password));
    if (it == _pools.end())
    {
        log_debug("pool for url \"" << url << "\" username \"" << username << "\" not found");
    }
    else
    {
        it->second->drop(keep);
        if (it->second->getCurrentSize() == 0)
        {
            log_debug("delete connectionpool for url \"" << url << "\", username \"" << username << '"');
            _pools.erase(it);
        }
    }
}

unsigned ConnectionPools::getCurrentSize(const std::string& url, const std::string& username, const std::string& password) const
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto it = _pools.find(ConnectionParameter(url, username, password));
    return it == _pools.end() ? 0
                             : it->second->getCurrentSize();
}

unsigned ConnectionPools::getCurrentSize() const
{
    std::lock_guard<std::mutex> lock(_mutex);

    unsigned size = 0;
    for (auto it = _pools.begin(); it != _pools.end(); ++it)
        size += it->second->getCurrentSize();

    return size;
}

void ConnectionPools::setMaxSpare(unsigned m)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _maxcount = m;
    for (auto it = _pools.begin(); it != _pools.end(); ++it)
        it->second->setMaxSpare(m);
}
}
