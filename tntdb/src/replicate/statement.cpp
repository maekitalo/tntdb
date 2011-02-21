/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <tntdb/replicate/statement.h>
#include <tntdb/replicate/connection.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>

namespace tntdb
{
  namespace replicate
  {
    Statement::Statement(Connection* conn, const std::string& query)
    {
      for (Connection::Connections::iterator it = conn->connections.begin(); it != conn->connections.end(); ++it)
        statements.push_back(it->prepare(query));
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
      for (Statements::iterator it = statements.begin(); it != statements.end(); ++it)
        it->execute();
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
