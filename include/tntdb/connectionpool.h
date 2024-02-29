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

#ifndef TNTDB_CONNECTIONPOOL_H
#define TNTDB_CONNECTIONPOOL_H

#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <memory>

namespace tntdb
{
class Connection;
class IConnection;

class ConnectionPool
{
    friend class PoolConnection;

    std::string _url;
    std::string _username;
    std::string _password;
    mutable std::mutex _mutex;

    std::vector<std::shared_ptr<IConnection>> _connectionPool;
    unsigned _maxSpare;
    void put(std::shared_ptr<IConnection>& conn);

    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

public:
    explicit ConnectionPool(const std::string& url, const std::string& username, const std::string& password, unsigned maxSpare = 0)
        : _url(url),
          _username(username),
          _password(password),
          _maxSpare(maxSpare)
        { }

    Connection connect();

    /// Release unused connections; keep the given number of connections
    void drop(unsigned keep = 0);

    unsigned getMaxSpare() const    { return _maxSpare; }
    void setMaxSpare(unsigned m);
    unsigned getCurrentSize() const { return _connectionPool.size(); }
};

class ConnectionPools
{
    ConnectionPools(const ConnectionPools&) = default;
    ConnectionPools& operator=(const ConnectionPools&) = default;

public:
    struct ConnectionParameter
    {
        std::string url;
        std::string username;
        std::string password;

        ConnectionParameter() { }
        ConnectionParameter(const std::string& url_, const std::string& username_, const std::string& password_)
            : url(url_),
              username(username_),
              password(password_)
        { }

        bool operator< (const ConnectionParameter& other) const
        {
            int c;

            c = url.compare(other.url);
            if (c != 0)
                return c < 0;

            c = username.compare(other.username);
            if (c != 0)
                return c < 0;

            return password < other.password;
        }
    };

    typedef ConnectionPool PoolType;
    typedef std::map<ConnectionParameter, std::unique_ptr<PoolType>> PoolsType;

private:
    PoolsType _pools;
    unsigned _maxcount;
    mutable std::mutex _mutex;

public:
    explicit ConnectionPools(unsigned maxcount = 0)
      : _maxcount(maxcount)
      { }

    Connection connect(const std::string& url, const std::string& username, const std::string& password);

    /// Release unused connections per pool; keep the given number of connections
    void drop(unsigned keep = 0);

    /** Release unused connections with the given database url;
        keep the given number of connections
     */
    void drop(const std::string& url, const std::string& username, const std::string& password, unsigned keep = 0);

    unsigned getMaximumSize()
        { return _maxcount; }

    void setMaxSpare(unsigned m);
    unsigned getCurrentSize(const std::string& url, const std::string& username, const std::string& password) const;
    unsigned getCurrentSize() const;
};
}

#endif // TNTDB_CONNECTIONPOOL_H

