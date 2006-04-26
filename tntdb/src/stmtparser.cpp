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

#include <tntdb/stmtparser.h>
#include <cxxtools/log.h>

log_define("tntdb.stmtparser")

namespace tntdb
{
  void StmtParser::parse(const std::string& sqlIn, StmtEvent& event)
  {
    {
      enum state_type {
        STATE_0,
        STATE_NAME0,
        STATE_NAME,
        STATE_STRING,
        STATE_STRING_ESC,
        STATE_ESC
      } state = STATE_0;

      std::string name;
      sql.clear();
      char end_token;

      log_debug("parse sql \"" << sqlIn << "\"");

      for (std::string::const_iterator it = sqlIn.begin();
           it != sqlIn.end(); ++it)
      {
        char ch = *it;
        switch(state)
        {
          case STATE_0:
            if (ch == ':')
              state = STATE_NAME0;
            else if (ch == '\\')
              state = STATE_ESC;
            else
            {
              sql += ch;
              if (ch == '\'' || ch == '"' || ch == '`')
              {
                state = STATE_STRING;
                end_token = ch;
              }
            }
            break;

          case STATE_NAME0:
            if (std::isalpha(ch))
            {
              name = ch;
              state = STATE_NAME;
            }
            else if (ch == ':')
              sql += ':';
            else if (ch == '\\')
            {
              sql += ':';
              state = STATE_ESC;
            }
            break;

          case STATE_NAME:
            if (std::isalnum(ch) || ch == '_')
              name += ch;
            else
            {
              log_debug("hostvar :" << name);
              sql += event.onHostVar(name);
              if (ch == '\\')
                state = STATE_ESC;
              else
              {
                sql += ch;
                state = STATE_0;
              }
            }
            break;

          case STATE_STRING:
            sql += ch;
            if (ch == end_token)
              state = STATE_0;
            else if (ch == '\\')
              state = STATE_STRING_ESC;
            break;

          case STATE_STRING_ESC:
            sql += ch;
            state = STATE_STRING;
            break;

          case STATE_ESC:
            sql += ch;
            state = STATE_0;
            break;
        }
      }

      switch(state)
      {
        case STATE_NAME0:
          sql += ':';
          break;

        case STATE_NAME:
          log_debug("hostvar :" << name);
          sql += event.onHostVar(name);
          break;

        default:
          ;
      }
    }
  }
}
