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

#include <tntdb/sqlbuilder.h>
#include <tntdb/stmtparser.h>
#include <cxxtools/convert.h>

namespace tntdb
{
  namespace
  {
    class HvarListReplacer : public StmtEvent
    {
        std::string varname;
        unsigned count;

      public:
        HvarListReplacer(const std::string& varname_, unsigned count_)
          : varname(varname_),
            count(count_)
        { }

        virtual std::string onHostVar(const std::string& name);
    };

    std::string HvarListReplacer::onHostVar(const std::string& name)
    {
      if (name != varname)
        return '%' + name;

      if (count == 0)
        return " NULL ";

      std::string ret;
      ret.reserve(2 + (5 + varname.size()) * count);
      ret = ' ';
      for (unsigned n = 0; n < count; ++n)
      {
        if (n != 0)
          ret += ',';
        ret += ':';
        ret += varname;
        ret += cxxtools::convert<std::string>(n);
      }
      ret += ' ';

      return ret;
    }

    class SimpleReplacer : public StmtEvent
    {
        std::string varname;
        std::string value;

      public:
        SimpleReplacer(const std::string& varname_, const std::string& value_)
          : varname(varname_),
            value(value_)
        { }

        virtual std::string onHostVar(const std::string& name);
    };

    std::string SimpleReplacer::onHostVar(const std::string& name)
    {
      return name == varname ? value : '%' + name;
    }
  }

  SqlBuilder& SqlBuilder::extendParam(const std::string& varname, unsigned count)
  {
    StmtParser p('%');
    HvarListReplacer r(varname, count);
    p.parse(sql, r);
    sql = p.getSql();
    return *this;
  }

  SqlBuilder& SqlBuilder::replace(const std::string& varname, const std::string& value)
  {
    StmtParser p('%');
    SimpleReplacer r(varname, value);
    p.parse(sql, r);
    sql = p.getSql();
    return *this;
  }

}
