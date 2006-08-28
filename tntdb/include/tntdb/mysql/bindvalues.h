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

#ifndef TNTDB_MYSQL_BINDVALUES_H
#define TNTDB_MYSQL_BINDVALUES_H

#include <mysql.h>
#include <string>
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
        void setInt(unsigned n, int data)
          { mysql::setInt(values[n], data); }
        void setUnsigned(unsigned n, unsigned data)
          { mysql::setUnsigned(values[n], data); }
        void setFloat(unsigned n, float data)
          { mysql::setFloat(values[n], data); }
        void setDouble(unsigned n, double data)
          { mysql::setDouble(values[n], data); }
        void setChar(unsigned n, char data)
          { mysql::setChar(values[n], data); }
        void setString(unsigned n, const char* data)
          { mysql::setString(values[n], bindAttributes[n].length, data); }
        void setString(unsigned n, const std::string& data)
          { mysql::setString(values[n], bindAttributes[n].length, data); }
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
        long getLong(unsigned n) const
          { return mysql::getLong(values[n]); }
        float getFloat(unsigned n) const
          { return mysql::getFloat(values[n]); }
        double getDouble(unsigned n) const
          { return mysql::getDouble(values[n]); }
        char getChar(unsigned n) const
          { return mysql::getChar(values[n]); }
        std::string getString(unsigned n) const
          { return mysql::getString(values[n]); }

        void initOutBuffer(unsigned n, MYSQL_FIELD& f);
    };
  }
}

#endif // TNTDB_MYSQL_BINDVALUES_H

