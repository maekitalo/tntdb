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

#include <tntdb/sqlite/impl/statement.h>
#include <tntdb/sqlite/impl/cursor.h>
#include <tntdb/sqlite/impl/connection.h>
#include <tntdb/impl/result.h>
#include <tntdb/impl/row.h>
#include <tntdb/impl/value.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <sstream>
#include <limits>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.statement")

namespace tntdb
{
  namespace sqlite
  {
    Statement::Statement(Connection* conn_, const std::string& query_)
      : stmt(0),
        stmtInUse(0),
        conn(conn_),
        query(query_),
        needReset(false)
    {
    }

    Statement::~Statement()
    {
      if (stmt)
      {
        log_debug("sqlite3_finalize(" << stmt << ')');
        ::sqlite3_finalize(stmt);
      }

      if (stmtInUse && stmtInUse != stmt)
      {
        log_debug("sqlite3_finalize(" << stmtInUse << ')');
        ::sqlite3_finalize(stmtInUse);
      }
    }

    sqlite3_stmt* Statement::getBindStmt()
    {
      if (stmt == 0)
      {
        // hostvars don't need to be parsed, because sqlite accepts the hostvar-
        // syntax of tntdb (:vvv)

        // prepare statement
        const char* tzTail;
        log_debug("sqlite3_prepare(" << conn->getSqlite3() << ", \"" << query
          << "\", " << &stmt << ", " << &tzTail << ')');
        int ret = ::sqlite3_prepare(conn->getSqlite3(), query.data(), query.size(), &stmt, &tzTail);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_prepare", conn->getSqlite3(), ret);

        log_debug("sqlite3_stmt = " << stmt);

        if (stmtInUse)
        {
          // get bindings from stmtInUse
          log_debug("sqlite3_transfer_bindings(" << stmtInUse << ", " << stmt << ')');
          ret = ::sqlite3_transfer_bindings(stmtInUse, stmt);
          if (ret != SQLITE_OK)
          {
            log_debug("sqlite3_finalize(" << stmt << ')');
            ::sqlite3_finalize(stmt);
            stmt = 0;
            throw Execerror("sqlite3_finalize", stmtInUse, ret);
          }
        }
      }
      else if (needReset)
        reset();

      return stmt;
    }

    void Statement::putback(sqlite3_stmt* stmt_)
    {
      if (stmt == 0)
      {
        stmt = stmt_; // thank you - we can use it
        if (stmtInUse == stmt_)
          stmtInUse = 0; // it is not in use any more
        needReset = true;
      }
      else
      {
        // we have already a new statement-handle - destroy the old one
        log_debug("sqlite3_finalize(" << stmt_ << ')');
        ::sqlite3_finalize(stmt_);

        if (stmtInUse == stmt_)
          stmtInUse = 0;
      }
    }

    int Statement::getBindIndex(const std::string& col)
    {
      sqlite3_stmt* stmt = getBindStmt();

      log_debug("sqlite3_bind_parameter_index(" << stmt << ", :" << col  << ')');
      int idx = ::sqlite3_bind_parameter_index(stmt, (':' + col).c_str());
      if (idx == 0)
        log_warn("hostvariable :" << col << " not found");
      return idx;
    }

    void Statement::reset()
    {
      if (stmt)
      {
        if (needReset)
        {
          log_debug("sqlite3_reset(" << stmt << ')');
          int ret = ::sqlite3_reset(stmt);

          if (ret != SQLITE_OK)
            throw Execerror("sqlite3_reset", stmt, ret);

          needReset = false;
        }
      }
      else
        getBindStmt();
    }

    void Statement::clear()
    {
      sqlite3_stmt* stmt = getBindStmt();
      int count = ::sqlite3_bind_parameter_count(stmt);
      for (int i = 0; i < count; ++i)
      {
        int ret = ::sqlite3_bind_null(stmt, i + 1);
        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_null", stmt, ret);
      }
    }

    void Statement::setNull(const std::string& col)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_null(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_null(stmt, idx);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_null", stmt, ret);
      }
    }

    void Statement::setBool(const std::string& col, bool data)
    {
      setInt(col, data ? 1 : 0);
    }

    void Statement::setShort(const std::string& col, short data)
    {
      setInt(col, data);
    }

    void Statement::setInt(const std::string& col, int data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_int(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_int", stmt, ret);
      }
    }

    void Statement::setLong(const std::string& col, long data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_int64(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int64(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_int", stmt, ret);
      }
    }

    void Statement::setInt32(const std::string& col, int32_t data)
    {
      setInt(col, data);
    }

    void Statement::setUnsignedShort(const std::string& col, unsigned short data)
    {
      if (data > static_cast<unsigned short>(std::numeric_limits<unsigned short>::max()))
      {
        log_warn("possible loss of precision while converting unsigned short " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
      }
      else
        setInt(col, static_cast<int>(data));
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
      if (data > static_cast<unsigned>(std::numeric_limits<int>::max()))
      {
        log_warn("possible loss of precision while converting unsigned " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
      }
      else
        setInt(col, static_cast<int>(data));
    }

    void Statement::setUnsignedLong(const std::string& col, unsigned long data)
    {
      if (data > static_cast<unsigned long>(std::numeric_limits<long>::max()))
      {
        log_warn("possible loss of precision while converting long unsigned " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
      }
      else
        setLong(col, static_cast<long>(data));
    }

    void Statement::setUnsigned32(const std::string& col, uint32_t data)
    {
      setUnsigned(col, data);
    }

    void Statement::setInt64(const std::string& col, int64_t data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_int64(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int64(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_int64", stmt, ret);
      }
    }
    
    void Statement::setUnsigned64(const std::string& col, uint64_t data)
    {
      setInt64(col, (int64_t)data);
    }
    
    void Statement::setDecimal(const std::string& col, const Decimal& data)
    {
      // SQLite 3.4.1 does not support the SQL decimal or numeric types.
      // So double is used instead, but of course binary floating point can
      // not accurately store decimal floating point numbers.
      // Maybe we could try instead using:
      // int sqlite3_value_numeric_type(sqlite3_value*);
      // However I do not understand the SQLite 3.4.1 documentation
      // for this API.  I am confused by how it only returns an int,
      // yet the API documentation talks about returning an int, double
      // or text string.
      double d = data.getDouble();
      setDouble(col, d);
    }
    
    void Statement::setFloat(const std::string& col, float data)
    {
      setDouble(col, static_cast<double>(data));
    }

    void Statement::setDouble(const std::string& col, double data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_double(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_double(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_double", stmt, ret);
      }
    }

    void Statement::setChar(const std::string& col, char data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
          << ", 1, SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_text(stmt, idx, &data, 1, SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_text", stmt, ret);
      }
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
          << ", " << data.size() << ", SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_text(stmt, idx, data.data(), data.size(), SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_text", stmt, ret);
      }
    }

    void Statement::setBlob(const std::string& col, const Blob& data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_blob(" << stmt << ", " << idx << ", data, "
            << data.size() << ", SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_blob(stmt, idx, data.data(), data.size(), SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_blob", stmt, ret);
      }
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
      setString(col, data.getIso());
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
      setString(col, data.getIso());
    }

    void Statement::setDatetime(const std::string& col, const Datetime& data)
    {
      setString(col, data.getIso());
    }

    Statement::size_type Statement::execute()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
        throw Execerror("sqlite3_step", stmt, ret);
      else if (ret != SQLITE_DONE)
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret << " from sqlite3_step";
        throw SqliteError("sqlite3_step", msg.str());
      }

      int n = ::sqlite3_changes(::sqlite3_db_handle(stmt));

      reset();

      return n;
    }

    Result Statement::select()
    {
      reset();
      needReset = true;

      ResultImpl* r = new ResultImpl();
      Result result(r);
      int ret;
      do
      {
        log_debug("sqlite3_step(" << stmt << ')');
        ret = sqlite3_step(stmt);

        if (ret == SQLITE_ERROR)
          throw Execerror("sqlite3_step", stmt, ret);
        else if (ret == SQLITE_ROW)
        {
          log_debug("sqlite3_column_count(" << stmt << ')');
          int count = ::sqlite3_column_count(stmt);
          RowImpl* row = new RowImpl();
          r->add(Row(row));
          for (int i = 0; i < count; ++i)
          {
            log_debug("sqlite3_column_bytes(" << stmt << ", " << i << ')');
            int n = sqlite3_column_bytes(stmt, i);

            const void* txt = 0;

            if (n > 0)
            {
              log_debug("sqlite3_column_blob(" << stmt << ", " << i << ')');
              txt = sqlite3_column_blob(stmt, i);
            }

            Value v;
            if (txt)
              v = Value(new ValueImpl(
                std::string(static_cast<const char*>(txt), n)));

            log_debug("sqlite3_column_name(" << stmt << ", " << i << ')');
            const char* name = sqlite3_column_name(stmt, i);
            if (name == 0)
              throw std::bad_alloc();

            row->add(name, v);
          }
        }
        else if (ret != SQLITE_DONE)
        {
          std::ostringstream msg;
          msg << "unexpected returncode " << ret;
          throw SqliteError("sqlite3_step", msg.str());
        }

      } while (ret == SQLITE_ROW);

      return result;
    }

    Row Statement::selectRow()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
        throw Execerror("sqlite3_step", stmt, ret);
      else if (ret == SQLITE_DONE)
        throw NotFound();
      else if (ret == SQLITE_ROW)
      {
        log_debug("sqlite3_column_count(" << stmt << ')');
        int count = ::sqlite3_column_count(stmt);
        RowImpl* r = new RowImpl();
        Row row(r);
        for (int i = 0; i < count; ++i)
        {
          log_debug("sqlite3_column_bytes(" << stmt << ", " << i << ')');
          int n = sqlite3_column_bytes(stmt, i);

          const void* txt = 0;

          if (n > 0)
          {
            log_debug("sqlite3_column_blob(" << stmt << ", " << i << ')');
            txt = sqlite3_column_blob(stmt, i);
          }

          Value v;
          if (txt)
            v = Value(new ValueImpl(
              std::string(static_cast<const char*>(txt), n)));

          log_debug("sqlite3_column_name(" << stmt << ", " << i << ')');
          const char* name = sqlite3_column_name(stmt, i);
          if (name == 0)
            throw std::bad_alloc();

          r->add(name, v);
        }
        return row;
      }
      else
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret;
        throw SqliteError("sqlite3_step", msg.str());
      }
    }

    Value Statement::selectValue()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
      {
        log_debug("sqlite3_step returned SQLITE_ERROR");
        throw Execerror("sqlite3_step", stmt, ret);
      }
      else if (ret == SQLITE_DONE)
      {
        log_debug("sqlite3_step returned SQLITE_DONE => NotFound");
        throw NotFound();
      }
      else if (ret == SQLITE_ROW)
      {
        log_debug("sqlite3_column_count(" << stmt << ')');
        int count = ::sqlite3_column_count(stmt);
        if (count == 0)
          throw NotFound();

        log_debug("sqlite3_column_bytes(" << stmt << ", 0)");
        int n = sqlite3_column_bytes(stmt, 0);

        const void* txt = 0;

        if (n > 0)
        {
          log_debug("sqlite3_column_blob(" << stmt << ", 0)");
          txt = sqlite3_column_blob(stmt, 0);
        }

        Value v;
        if (txt)
          v = Value(new ValueImpl(
            std::string(static_cast<const char*>(txt), n)));
        return v;
      }
      else
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret;
        throw SqliteError("sqlite3_step", msg.str());
      }
    }

    ICursor* Statement::createCursor(unsigned fetchsize)
    {
      stmtInUse = getBindStmt();
      stmt = 0;
      return new Cursor(this, stmtInUse);
    }

  }
}
