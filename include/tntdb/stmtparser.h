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

#ifndef TNTDB_STMTPARSER_H
#define TNTDB_STMTPARSER_H

#include <string>

namespace tntdb
{
class StmtEvent
{
public:
    virtual ~StmtEvent()  {}
    // returns replacementvalue
    virtual std::string onHostVar(const std::string& name) = 0;
};

/** Search host variables

    Host variables start with a ':' followed by alpha characters.
    For each variable found the event handler StmtEvent is called.
    The host variable is removed and replaced with the value returned
    by the event handler.

    The character '\' is treated as an escape character. It is removed
    and the following character is processed as is. Name parsing
    is stopped at '\', so if you need to put alpha characters directly
    after a host variable, you can escape the character following the
    host variables name.

    Strings enclosed in ', " or ` are skipped.
 */
class StmtParser
{
    char praefixChar;
    std::string sql;

public:
    explicit StmtParser(char praefixChar_ = ':')
      : praefixChar(praefixChar_)
      { }

    void parse(const std::string& sqlIn, StmtEvent& event);

    const std::string& getSql() const { return sql; }
};
}

#endif // TNTDB_STMTPARSER_H

