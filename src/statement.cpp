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


#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>

log_define("tntdb.statement")

namespace tntdb
{
Statement::size_type Statement::execute()
{
    log_trace("Statement::execute()");
    return _stmt->execute();
}

Result Statement::select()
{
    log_trace("Statement::select()");
    return _stmt->select();
}

Row Statement::selectRow()
{
    log_trace("Statement::selectRow()");
    return _stmt->selectRow();
}

Value Statement::selectValue()
{
    log_trace("Statement::selectValue()");
    return _stmt->selectValue();
}

Statement::const_iterator Statement::begin(unsigned fetchsize) const
{
    log_trace("Statement::begin(" << fetchsize << ')');
    return const_iterator(_stmt->createCursor(fetchsize));
}

void Statement::maxNumDelay(size_type n)
{
    _stmt->maxNumDelay(n);
}

Statement::size_type Statement::numDelayed() const
{
    return _stmt->numDelayed();
}

Statement::size_type Statement::flush()
{
    return _stmt->flush();
}

void IStatement::setUString(const std::string& col, const cxxtools::String& data)
{
    setString(col, cxxtools::Utf8Codec::encode(data));
}

void IStatement::maxNumDelay(unsigned /*n*/)
{
}

unsigned IStatement::numDelayed() const
{
    return 0;
}

Statement::size_type IStatement::flush()
{
    return 0;
}

}

