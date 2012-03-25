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

#ifndef TNTDB_MYSQL_BINDVALUES_H
#define TNTDB_MYSQL_BINDVALUES_H

#include <mysql.h>
#include <string>
#include <tntdb/decimal.h>
#include <tntdb/mysql/bindutils.h>

namespace tntdb
{
  namespace mysql
  {
    class BindValues
    {
        unsigned valuesSize;
        MYSQL_BIND* values;
        struct BindAttributes
        {
          unsigned long length;
          my_bool isNull;
          std::string name;
          BindAttributes()
            : length(0),
              isNull(true)
              { }
        }* bindAttributes;

        // non copyable
        BindValues(const BindValues&);
        BindValues& operator=(const BindValues&);

      public:
        explicit BindValues(unsigned n);
        BindValues();
        ~BindValues();

        MYSQL_BIND* getMysqlBind() const  { return values; }
        void setSize(unsigned n);
        unsigned getSize() const   { return valuesSize; }

        void setNull(unsigned n)
          { mysql::setNull(values[n]); }
        void setBool(unsigned n, bool data)
          { mysql::setBool(values[n], data); }
        void setShort(unsigned n, short data)
          { mysql::setShort(values[n], data); }
        void setInt(unsigned n, int data)
          { mysql::setInt(values[n], data); }
        void setLong(unsigned n, int data)
          { mysql::setLong(values[n], data); }
        void setUnsignedShort(unsigned n, unsigned short data)
          { mysql::setUnsignedShort(values[n], data); }
        void setUnsigned(unsigned n, unsigned data)
          { mysql::setUnsigned(values[n], data); }
        void setUnsignedLong(unsigned n, unsigned data)
          { mysql::setUnsignedLong(values[n], data); }
        void setInt32(unsigned n, int32_t data)
          { mysql::setInt32(values[n], data); }
        void setUnsigned32(unsigned n, uint32_t data)
          { mysql::setUnsigned32(values[n], data); }
        void setInt64(unsigned n, int64_t data)
          { mysql::setInt64(values[n], data); }
        void setUnsigned64(unsigned n, uint64_t data)
          { mysql::setUnsigned64(values[n], data); }
        void setDecimal(unsigned n, const Decimal& data)
          { mysql::setDecimal(values[n], bindAttributes[n].length, data); }
        void setFloat(unsigned n, float data)
          { mysql::setFloat(values[n], data); }
        void setDouble(unsigned n, double data)
          { mysql::setDouble(values[n], data); }
        void setChar(unsigned n, char data)
          { mysql::setChar(values[n], bindAttributes[n].length, data); }
        void setString(unsigned n, const char* data)
          { mysql::setString(values[n], bindAttributes[n].length, data); }
        void setString(unsigned n, const std::string& data)
          { mysql::setString(values[n], bindAttributes[n].length, data); }
        void setBlob(unsigned n, const Blob& data)
          { mysql::setBlob(values[n], bindAttributes[n].length, data); }
        void setDate(unsigned n, const Date& data)
          { mysql::setDate(values[n], data); }
        void setTime(unsigned n, const Time& data)
          { mysql::setTime(values[n], data); }
        void setDatetime(unsigned n, const Datetime& data)
          { mysql::setDatetime(values[n], data); }

        bool isNull(unsigned n) const
          { return mysql::isNull(values[n]); }
        bool getBool(unsigned n) const
          { return mysql::getBool(values[n]); }
        int getInt(unsigned n) const
          { return mysql::getInt(values[n]); }
        unsigned getUnsigned(unsigned n) const
          { return mysql::getUnsigned(values[n]); }
        int32_t getInt32(unsigned n) const
          { return mysql::getInt32(values[n]); }
        uint32_t getUnsigned32(unsigned n) const
          { return mysql::getUnsigned32(values[n]); }
        int64_t getInt64(unsigned n) const
          { return mysql::getInt64(values[n]); }
        uint64_t getUnsigned64(unsigned n) const
          { return mysql::getUnsigned64(values[n]); }
        Decimal getDecimal(unsigned n) const
          { return mysql::getDecimal(values[n]); }
        long getLong(unsigned n) const
          { return mysql::getLong(values[n]); }
        float getFloat(unsigned n) const
          { return mysql::getFloat(values[n]); }
        double getDouble(unsigned n) const
          { return mysql::getDouble(values[n]); }
        char getChar(unsigned n) const
          { return mysql::getChar(values[n]); }
        void getString(unsigned n, std::string& ret) const
          { mysql::getString(values[n], ret); }

        const std::string& getName(unsigned n) const
          { return bindAttributes[n].name; }

        void initOutBuffer(unsigned n, MYSQL_FIELD& f);
        void clear();
    };
  }
}

#endif // TNTDB_MYSQL_BINDVALUES_H

