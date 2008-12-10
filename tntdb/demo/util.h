/*
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
////////////////////////////////////////////////////////////////////////
// util.h
//

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <iostream>
#include <tntdb/value.h>
#include <tntdb/row.h>

class ValuePrinter
{
    std::ostream& out;
    char delimiter;

  public:
    explicit ValuePrinter(std::ostream& out_, char delimiter_ = '\t')
      : out(out_), delimiter(delimiter_)
      { }
    explicit ValuePrinter(char delimiter_ = '\t')
      : out(std::cout), delimiter(delimiter_)
      { }

    void operator() (const tntdb::Value& v) const
    {
      if (v.isNull())
        out << "null" << delimiter;
      else
        out << v.getString() << delimiter;
    }
};

class RowPrinter
{
    std::ostream& out;
    char delimiter;

  public:
    explicit RowPrinter(std::ostream& out_, char delimiter_ = '\n')
      : out(out_),
        delimiter(delimiter_)
      { }

    explicit RowPrinter(char delimiter_ = '\n')
      : out(std::cout),
        delimiter(delimiter_)
      { }

    void operator() (const tntdb::Row& t) const
    {
      std::for_each(t.begin(), t.end(),
        ValuePrinter(out, '\t'));
      out << delimiter;
    }
};

#endif // UTIL_H

