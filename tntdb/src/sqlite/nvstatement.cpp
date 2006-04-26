////////////////////////////////////////////////////////////////////////
// sqlite/nvstatement.cpp
//

#include <tntdb/sqlite/nvstatement.h>
#include <cctype>

namespace tntdb
{
  namespace sqlite
  {
    void Nvstatement::processHValue(std::string& ret, const std::string& name) const
    {
      values_type::const_iterator v = values.find(name);
      if (v != values.end())
      {
        if (v->second.isNull())
        {
          ret += "NULL";
        }
        else
        {
          ret += '\'';
          const std::string& val = v->second.getString();
          for (std::string::const_iterator it = val.begin();
               it != val.end(); ++it)
            if (*it == '\'')
              ret += "''";
            else
              ret += *it;
          ret += '\'';
        }
      }
      else
      {
        ret += ':';
        ret += name;
      }
    }

    std::string Nvstatement::getSql() const
    {
      enum state_type {
        STATE_0,
        STATE_NAME0,
        STATE_NAME,
        STATE_ESC
      } state = STATE_0;

      std::string ret;
      std::string name;

      for (std::string::const_iterator it = sql.begin();
           it != sql.end(); ++it)
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
              ret += ch;
            break;

          case STATE_NAME0:
            if (std::isalpha(ch))
            {
              name = ch;
              state = STATE_NAME;
            }
            else if (ch == ':')
              ret += ':';
            else if (ch == '\\')
            {
              ret += ':';
              state = STATE_ESC;
            }
            break;

          case STATE_NAME:
            if (std::isalnum(ch))
            {
              name += ch;
            }
            else
            {
              processHValue(ret, name);
              if (ch == '\\')
                state = STATE_ESC;
              else
              {
                ret += ch;
                state = STATE_0;
              }
            }
            break;

          case STATE_ESC:
            ret += ch;
            state = STATE_0;
            break;
        }
      }

      switch(state)
      {
        case STATE_NAME0:
          ret += ':';
          break;

        case STATE_NAME:
          processHValue(ret, name);
          break;

        default:
          ;
      }

      return ret;
    }

  }
}
