/* 
 * Copyright (C) 2005 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

