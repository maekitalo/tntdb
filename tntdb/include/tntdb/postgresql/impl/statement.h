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

#ifndef TNTDB_POSTGRESQL_IMPL_SQLSTATEMENT_H
#define TNTDB_POSTGRESQL_IMPL_SQLSTATEMENT_H

#include <tntdb/iface/istatement.h>
#include <tntdb/bits/connection.h>
#include <map>
#include <vector>
#include <libpq-fe.h>
#include <cxxtools/dynbuffer.h>

namespace tntdb
{
  namespace postgresql
  {
    class Connection;

    class Statement : public IStatement
    {
        Connection* conn;
        std::string query;
        std::string stmtName;
        typedef std::map<std::string, unsigned> hostvarMapType;
        hostvarMapType hostvarMap;

        class valueType
        {
            bool isNull;
            std::string value;
#ifndef HAVE_PQPREPARE
            std::string type;
#endif

          public:
#ifndef HAVE_PQPREPARE
            valueType()
              : type("text")
              { }
#endif
            void setNull()            { isNull = true; }
            void setValue(const std::string& v)
                                      { value = v; isNull = false; }
            const char* getValue()    { return isNull ? 0 : value.data(); }
            unsigned getLength()      { return isNull ? 0 : value.size(); }
#ifndef HAVE_PQPREPARE
            void setType(const std::string& t)   { type = t; }
            const std::string& getType() const   { return type; }
#endif
        };

        typedef std::vector<valueType> valuesType;
        valuesType values;

        cxxtools::Dynbuffer<const char*> paramValues;
        cxxtools::Dynbuffer<int> paramLengths;
        cxxtools::Dynbuffer<int> paramFormats;

        // helper-methods for setting values
        template <typename T>
        void setValue(const std::string& col, T data);

        template <typename T>
        void setStringValue(const std::string& col, T data, bool binary = false);

        template <typename T>
        void setIsoValue(const std::string& col, T data);

#ifndef HAVE_PQPREPARE
        void setType(const std::string& col, const std::string& type);
#endif

        void doPrepare();
        PGresult* execPrepared();

      public:
        Statement(Connection* conn, const std::string& query);
        ~Statement();

        // methods of IStatement

        void clear();
        void setNull(const std::string& col);
        void setBool(const std::string& col, bool data);
        void setInt(const std::string& col, int data);
        void setUnsigned(const std::string& col, unsigned data);
        void setInt32(const std::string& col, int32_t data);
        void setUnsigned32(const std::string& col, uint32_t data);
        void setInt64(const std::string& col, int64_t data);
        void setUnsigned64(const std::string& col, uint64_t data);
        void setDecimal(const std::string& col, const Decimal& data);
        void setFloat(const std::string& col, float data);
        void setDouble(const std::string& col, double data);
        void setChar(const std::string& col, char data);
        void setString(const std::string& col, const std::string& data);
        void setBlob(const std::string& col, const Blob& data);
        void setDate(const std::string& col, const Date& data);
        void setTime(const std::string& col, const Time& data);
        void setDatetime(const std::string& col, const Datetime& data);

        size_type execute();
        tntdb::Result select();
        tntdb::Row selectRow();
        tntdb::Value selectValue();
        ICursor* createCursor(unsigned fetchsize);

        // specific methods
        const std::string& getQuery() const   { return query; }
        int getNParams()               { return values.size(); }
        const char* const* getParamValues();
        const int* getParamLengths();
        const int* getParamFormats()
            { return paramFormats.data(); }
        PGconn* getPGConn();
        Connection* getConnection()    { return conn; }
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_SQLSTATEMENT_H

