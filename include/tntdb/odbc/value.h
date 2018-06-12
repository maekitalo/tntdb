/*
 * Copyright (C) 2018 Tommi Maekitalo
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

#ifndef TNTDB_ODBC_VALUE_H
#define TNTDB_ODBC_VALUE_H

#include <tntdb/iface/ivalue.h>

#include <sqltypes.h>

#include <vector>

namespace tntdb
{
    namespace odbc
    {
        class Value : public IValue
        {
            std::vector<char> _data;
            std::string _columnName;
            SQLLEN _lenOrInd;
            SQLSMALLINT _dataType;

            void bind(SQLHSTMT hStmt, SQLUSMALLINT columnNumber,
                SQLSMALLINT dataType, SQLULEN columnSize,
                SQLSMALLINT decimalDigits, SQLSMALLINT nullable);

        public:
            Value(SQLHSTMT hStmt, SQLUSMALLINT columnNumber);
            Value(SQLHSTMT hStmt, SQLUSMALLINT columnNumber, const std::string& columnName,
                SQLSMALLINT dataType, SQLULEN columnSize,
                SQLSMALLINT decimalDigits, SQLSMALLINT nullable);

            const std::string& columnName() const   { return _columnName; }

            virtual bool isNull() const;
            virtual bool getBool() const;
            virtual short getShort() const;
            virtual int getInt() const;
            virtual long getLong() const;
            virtual unsigned getUnsigned() const;
            virtual unsigned short getUnsignedShort() const;
            virtual unsigned long getUnsignedLong() const;
            virtual int32_t getInt32() const;
            virtual uint32_t getUnsigned32() const;
            virtual int64_t getInt64() const;
            virtual uint64_t getUnsigned64() const;
            virtual Decimal getDecimal() const;
            virtual float getFloat() const;
            virtual double getDouble() const;
            virtual char getChar() const;
            virtual void getString(std::string& ret) const;
            virtual void getBlob(Blob& ret) const;
            virtual Date getDate() const;
            virtual Time getTime() const;
            virtual Datetime getDatetime() const;
            virtual void getUString(cxxtools::String& ret) const;
        };
    }
}
#endif