/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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

  /* Searches hostvariables.
   *
   * Hostvariables start with a ':' followed by alpha-characters.
   * For each found variable the eventhandler StmtEvent is called.
   * The hostvariable is removed and replaced with the value returned
   * by the eventhandler.
   *
   * The character '\' is treated as a escape-character. It is removed
   * and the following character is processed as is. Nameparsing
   * is stopped at '\', so if you need to put alpha-characters directly
   * after a hostvariable, you can escape the character following the
   * hostvariables name.
   * 
   * Strings enclosed in ', " or ` are skipped.
   */
  class StmtParser
  {
      std::string sql;

    public:
      void parse(const std::string& sqlIn, StmtEvent& event);

      const std::string& getSql() const   { return sql; }
  };

}

#endif // TNTDB_STMTPARSER_H

