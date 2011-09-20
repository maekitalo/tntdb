/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <tntdb/replicate/statement.h>
#include <tntdb/replicate/connection.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/transaction.h>
#include <cxxtools/log.h>

log_define("tntdb.replicate.statement")

namespace tntdb
{
  namespace replicate
  {
    Statement::Statement(Connection* conn_, const std::string& query)
      : conn(conn_)
    {
      // check if it a select statement
      // a select statement need to be prepared only on the first connection

      // skip white space first
      const char* p = query.c_str();
      while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
        ++p;

      if (strncasecmp(p, "select", 6) == 0)
      {
        log_debug("select statement detected - prepare on first connection only");
        statements.push_back(conn->connections.begin()->prepare(query));
      }
      else
      {
        log_debug("non-select statement detected - prepare on all " << conn->connections.size() << " connections");
        for (Connection::Connections::iterator it = conn->connections.begin(); it != conn->connections.end(); ++it)
          statements.push_back(it->prepare(query));
      }
    }

    void Statement::clear()
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->clear();
    }

    void Statement::setNull(const std::string& col)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setNull(col);
    }

    void Statement::setBool(const std::string& col, bool data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setBool(col, data);
    }

    void Statement::setShort(const std::string& col, short data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setShort(col, data);
    }

    void Statement::setInt(const std::string& col, int data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setInt(col, data);
    }

    void Statement::setLong(const std::string& col, long data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setLong(col, data);
    }

    void Statement::setUnsignedShort(const std::string& col, unsigned short data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setUnsignedShort(col, data);
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setUnsigned(col, data);
    }

    void Statement::setUnsignedLong(const std::string& col, unsigned long data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setUnsignedLong(col, data);
    }

    void Statement::setInt32(const std::string& col, int32_t data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setInt32(col, data);
    }

    void Statement::setUnsigned32(const std::string& col, uint32_t data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setUnsigned32(col, data);
    }

    void Statement::setInt64(const std::string& col, int64_t data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setInt64(col, data);
    }

    void Statement::setUnsigned64(const std::string& col, uint64_t data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setUnsigned64(col, data);
    }

    void Statement::setDecimal(const std::string& col, const Decimal& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setDecimal(col, data);
    }

    void Statement::setFloat(const std::string& col, float data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setFloat(col, data);
    }

    void Statement::setDouble(const std::string& col, double data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setDouble(col, data);
    }

    void Statement::setChar(const std::string& col, char data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setChar(col, data);
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setString(col, data);
    }

    void Statement::setBlob(const std::string& col, const Blob& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setBlob(col, data);
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setDate(col, data);
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setTime(col, data);
    }

    void Statement::setDatetime(const std::string& col, const Datetime& data)
    {
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->setDatetime(col, data);
    }

    Statement::size_type Statement::execute()
    {
      tntdb::Connection c(conn);
      Transaction transaction(c);
      Statement::size_type ret = 0;
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
      {
        Statement::size_type r = it->execute();
        if (it == statements.begin())
          ret = r;
      }
      transaction.commit();
      return ret;
    }

    tntdb::Result Statement::select()
    {
      return statements.begin()->select();
    }

    tntdb::Row Statement::selectRow()
    {
      return statements.begin()->selectRow();
    }

    tntdb::Value Statement::selectValue()
    {
      return statements.begin()->selectValue();
    }

    ICursor* Statement::createCursor(unsigned fetchsize)
    {
      return statements.begin()->getImpl()->createCursor(fetchsize);
    }

  }
}
