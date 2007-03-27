/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include <tntdb/mysql/impl/connection.h>
#include <tntdb/mysql/impl/result.h>
#include <tntdb/mysql/impl/statement.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>
#include <tntdb/mysql/error.h>

#include <cxxtools/log.h>

log_define("tntdb.mysql.connection");

namespace tntdb
{
  namespace mysql
  {
    namespace
    {
      std::string str(const char* s)
      {
        return s && *s ? std::string("\"") + s + '"'
                       : std::string("null");
      }
      const char* zstr(const char* s)
      { return s && s[0] ? s : 0; }
    }

    void Connection::open(const char* host, const char* user,
      const char* passwd, const char* db, unsigned int port,
      const char* unix_socket, unsigned long client_flag)
    {
      log_debug("mysql_real_connect(MYSQL, "
        << str(host) << ", "
        << str(user) << ", "
        << str(passwd) << ", "
        << str(db) << ", "
        << port << ", "
        << str(unix_socket) << ", "
        << client_flag << ')');

      if (::mysql_init(&mysql) == 0)
        throw std::runtime_error("cannot initalize mysql");
      initialized = true;

      if (!::mysql_real_connect(&mysql, zstr(host), zstr(user), zstr(passwd),
                                zstr(db), port, zstr(unix_socket), client_flag))
        throw MysqlError("mysql_real_connect", &mysql);
    }

    Connection::Connection(const char* host, const char* user,
      const char* passwd, const char* db, unsigned int port,
      const char* unix_socket, unsigned long client_flag)
      : initialized(false)
    {
      open(host, user, passwd, db, port, unix_socket, client_flag);
    }

    Connection::Connection(const char* conn)
      : initialized(false)
    {
      log_debug("Connection::Connection(\"" << conn << "\")");
      std::string host;
      std::string user;
      std::string passwd;
      std::string db;
      unsigned int port = 3306;
      std::string unix_socket;
      unsigned long client_flag = 0;

      enum state_type {
        state_key,
        state_value,
        state_port,
        state_flag,
      } state = state_key;

      std::string key;
      std::string* value;

      for (const char* p = conn; *p; ++p)
      {
        switch (state)
        {
          case state_key:
            if (*p == '=')
            {
              if (key == "port")
              {
                port = 0;
                key.clear();
                state = state_port;
              }
              else if (key == "flags")
              {
                key.clear();
                state = state_flag;
              }
              else
              {
                if (key == "host")
                  value = &host;
                else if (key == "user")
                  value = &user;
                else if (key == "passwd")
                  value = &passwd;
                else if (key == "db")
                  value = &db;
                else if (key == "unix_socket")
                  value = &unix_socket;
                else
                  throw std::runtime_error("invalid key \"" + key
                    + "\" in connectionstring \"" + conn + '"');

                key.clear();
                state = state_value;
              }
            }
            else
              key += *p;
            break;

          case state_value:
            if (*p == ';')
              state = state_key;
            else
              *value += *p;
            break;

          case state_port:
            if (*p == ';')
              state = state_key;
            else if (std::isdigit(*p))
              port = port * 10 + (*p - '0');
            else
              throw std::runtime_error(
                std::string("invalid port in connectionstring ") + conn);
            break;

          case state_flag:
            if (*p == ';')
              state = state_key;
            else if (std::isdigit(*p))
              client_flag = client_flag * 10 + (*p - '0');
            else
              throw std::runtime_error(
                std::string("invalid flag in connectionstring ") + conn);
            break;
        }
      }

      if (state == state_key && !key.empty())
        throw std::runtime_error(std::string("invalid connectionstring ") + conn);

      open(host.c_str(), user.c_str(), passwd.c_str(), db.c_str(),
        port, unix_socket.c_str(), client_flag);
    }

    Connection::~Connection()
    {
      if (initialized)
      {
        clearStatementCache();

        log_debug("mysql_close(" << &mysql << ')');
        ::mysql_close(&mysql);
      }
    }

    void Connection::beginTransaction()
    {
      log_debug("mysql_autocomit(" << &mysql << ", " << 0 << ')');
      if (::mysql_autocommit(&mysql, 0) != 0)
        throw MysqlError("mysql_autocommit", &mysql);
    }

    void Connection::commitTransaction()
    {
      log_debug("mysql_commit(" << &mysql << ')');
      if (::mysql_commit(&mysql) != 0)
        throw MysqlError("mysql_commit", &mysql);

      log_debug("mysql_autocomit(" << &mysql << ", " << 1 << ')');
      if (::mysql_autocommit(&mysql, 1) != 0)
        throw MysqlError("mysql_autocommit", &mysql);
    }

    void Connection::rollbackTransaction()
    {
      log_debug("mysql_rollback(" << &mysql << ')');
      if (::mysql_rollback(&mysql) != 0)
        throw MysqlError("mysql_rollback", &mysql);

      log_debug("mysql_autocommit(" << &mysql << ", " << 1 << ')');
      if (::mysql_autocommit(&mysql, 1) != 0)
        throw MysqlError("mysql_autocommit", &mysql);
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
      log_debug("mysql_query(\"" << query << "\")");
      if (::mysql_query(&mysql, query.c_str()) != 0)
        throw MysqlError("mysql_query", &mysql);

      log_debug("mysql_affected_rows(" << &mysql << ')');
      return ::mysql_affected_rows(&mysql);
    }

    tntdb::Result Connection::select(const std::string& query)
    {
      execute(query);

      log_debug("mysql_store_result(" << &mysql << ')');
      MYSQL_RES* res = ::mysql_store_result(&mysql);
      if (res == 0)
        throw MysqlError("mysql_store_result", &mysql);

      return tntdb::Result(new Result(tntdb::Connection(this), &mysql, res));
    }

    Row Connection::selectRow(const std::string& query)
    {
      tntdb::Result result = select(query);
      if (result.empty())
        throw NotFound();

      return result.getRow(0);
    }

    Value Connection::selectValue(const std::string& query)
    {
      Row t = selectRow(query);
      if (t.empty())
        throw NotFound();

      return t.getValue(0);
    }

    tntdb::Statement Connection::prepare(const std::string& query)
    {
      return tntdb::Statement(new Statement(tntdb::Connection(this), &mysql, query));
    }

    bool Connection::ping()
    {
      int ret = ::mysql_ping(&mysql);
      log_debug("mysql_ping() => " << ret);
      return ret == 0;
    }

    my_ulonglong Connection::getInsertId()
    {
      log_debug("mysql_insert_id(" << &mysql << ')');
      return ::mysql_insert_id(&mysql);
    }
  }
}
