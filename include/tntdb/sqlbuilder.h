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

#ifndef TNTDB_SQLBUILDER_H
#define TNTDB_SQLBUILDER_H

#include <string>

namespace tntdb
{
  class SqlBuilder
  {
    public:
      SqlBuilder()
      { }

      SqlBuilder(const std::string& sql_)
        : sql(sql_)
      { }

      SqlBuilder& extendParam(const std::string& varname, unsigned count);
      SqlBuilder& replace(const std::string& varname, const std::string& value);
      SqlBuilder& replaceIf(bool condition, const std::string& varname,
          const std::string& value, const std::string& elsevalue = std::string())
      { replace(varname, condition ? value : elsevalue); return *this; }

      void str(const std::string& s)
      { sql = s; }

      const std::string& str() const
      { return sql; }

      operator const std::string&() const
      { return sql; }

    private:
      std::string sql;
  };
}

#endif // TNTDB_SQLBUILDER_H
