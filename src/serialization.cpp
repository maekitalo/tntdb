/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#include <tntdb/serialization.h>
#include <tntdb/value.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>
#include <cxxtools/serializationinfo.h>

namespace tntdb
{
    void operator<<= (cxxtools::SerializationInfo& si, const tntdb::Value& value)
    {
        si.setTypeName("dbValue");

        std::string s;
        if (value.get(s))
            si <<= value;
        else
            si.setNull();
    }

    void operator<<= (cxxtools::SerializationInfo& si, const tntdb::Row& row)
    {
        si.setTypeName("dbRow");

        for (unsigned n = 0; n < row.size(); ++n)
        {
            cxxtools::SerializationInfo& vsi = si.addMember(row.getName(n));

            // leave the new member NULL if the value is null
            std::string value;
            if (row[n].get(value))
                vsi <<= value;
        }
    }

    void operator<<= (cxxtools::SerializationInfo& si, const tntdb::Result& res)
    {
        si.setTypeName("dbResult");
        si.setCategory(cxxtools::SerializationInfo::Array);

        for (auto cur: res)
            si.addMember() <<= cur;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const tntdb::Statement& res)
    {
        si.setTypeName("dbResult");
        si.setCategory(cxxtools::SerializationInfo::Array);

        for (auto cur: res)
            si.addMember() <<= cur;
    }

    void operator>>= (const cxxtools::SerializationInfo& si, Statement& stmt)
    {
        for (cxxtools::SerializationInfo::const_iterator it = si.begin();
                it != si.end(); ++it)
        {
            if (it->isNull())
            {
                stmt.setNull(it->name());
            }
            else if (it->isString8())
            {
                std::string value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else if (it->isChar())
            {
                char value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else if (it->isBool())
            {
                bool value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else if (it->isInt())
            {
                long value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else if (it->isUInt())
            {
                unsigned long value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else if (it->isFloat())
            {
                double value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
            else
            {
                cxxtools::String value;
                it->getValue(value);
                stmt.set(it->name(), value);
            }
        }
    }

}
