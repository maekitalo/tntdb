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

#ifndef TNTDB_SERIALIZATION_H
#define TNTDB_SERIALIZATION_H

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntdb
{
    class Value;
    class Row;
    class Result;
    class Statement;

    /** Makes tntdb::Value serializable in cxxtools serialization framework
     */
    void operator<<= (cxxtools::SerializationInfo& si, const Value& value);

    /** Makes tntdb::Row serializable in cxxtools serialization framework
     */
    void operator<<= (cxxtools::SerializationInfo& si, const Row& row);

    /** Makes tntdb::Result serializable in cxxtools serialization framework
     */
    void operator<<= (cxxtools::SerializationInfo& si, const Result& res);

    /** Makes tntdb::Statement serializable in cxxtools serialization framework
     */
    void operator<<= (cxxtools::SerializationInfo& si, const Statement& res);

    /** Serialize operator for tntdb::Statement.
     */
    void operator>>= (const cxxtools::SerializationInfo& si, Statement& stmt);

    /** Serialize objects into tntdb::Statement.
     *  This sets the placeholder variables in statement to the value of the
     *  object to serialize.
     *  
     *  Example:
     *  /code
     *    MyObject obj = ...;
     *    stmt = db.prepare("insert into ...");
     *    stmt <<= obj;   // this serializes MyObject into the statement
     *                    // by using the serialization operator of MyObject
     *                    // and setting the members in the statement one by one
     *
     *    stmt.execute();
     *  /endcode
     */
    template <typename T>
    Statement& operator<<= (Statement& stmt, const T& obj)
    {
        cxxtools::SerializationInfo si;
        si <<= obj;
        si >>= stmt;
        return stmt;
    }

    template <typename T>
    void operator>>= (const Row& row, T& obj)
    {
        cxxtools::SerializationInfo si;
        si <<= row;
        si >>= obj;
    }

}

#endif // TNTDB_SERIALIZATION_H
