/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include <tntdb/postgresql/impl/statement.h>
#include <tntdb/postgresql/impl/connection.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/cursor.h>
#include <tntdb/postgresql/error.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <tntdb/stmtparser.h>
#include <sstream>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/log.h>
#include "config.h"

log_define("tntdb.postgresql.statement")

#ifdef HAVE_PQPREPARE
#  define SET_TYPE(pos, type)
#else
#  define SET_TYPE(pos, type)  setType(pos, type)
#endif

namespace tntdb
{
  namespace postgresql
  {
    typedef std::map<std::string, unsigned> hostvarMapType;

    namespace
    {
      class SE : public StmtEvent
      {
          hostvarMapType& hostvarMap;
          unsigned idx;

        public:
          SE(hostvarMapType& hm)
            : hostvarMap(hm),
              idx(0)
            { }
          std::string onHostVar(const std::string& name);
          unsigned getMaxIdx() const  { return idx; }
      };

      std::string SE::onHostVar(const std::string& name)
      {
        unsigned n;

        hostvarMapType::const_iterator it = hostvarMap.find(name);
        if (it == hostvarMap.end())
        {
          n = idx++;
          hostvarMap[name] = n;
        }
        else
          n = it->second;

        log_debug("hostvar :" << name << " => $" << (n + 1));

        std::ostringstream r;
        r << '$' << (n + 1);
        return r.str();
      }
    }

    Statement::Statement(Connection* conn_, const std::string& query_)
      : conn(conn_)
    {
      // parse hostvars
      StmtParser parser;
      SE se(hostvarMap);
      parser.parse(query_, se);

      values.resize(se.getMaxIdx());

      query = parser.getSql();

      paramValues.reserve(se.getMaxIdx());
      paramLengths.reserve(se.getMaxIdx());
    }

    Statement::~Statement()
    {
      if (!stmtName.empty())
      {
        std::string sql = "DEALLOCATE " + stmtName;

        log_debug("PQexec(" << getPGConn() << ", \"" << sql << "\")");
        PGresult* result = PQexec(getPGConn(), sql.c_str());

        if (isError(result))
          log_error("error deallocating statement: " << PQresultErrorMessage(result));

        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }

    void Statement::doPrepare()
    {
      // create statementname
      std::ostringstream s;
      s << "tntdbstmt" << this;

      // prepare statement
#ifdef HAVE_PQPREPARE
      log_debug("PQprepare(" << getPGConn() << ", \"" << s.str()
        << "\", \"" << query << "\", 0, 0)");
      PGresult* result = PQprepare(getPGConn(),
        s.str().c_str(), query.c_str(), 0, 0);

      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(query, "PQprepare", result, true);
      }
#else
      std::ostringstream sql;
      sql << "PREPARE " << s.str();
      for (valuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        sql << (it == values.begin() ? " (" : ", " ) << it->getType();
      if (!values.empty())
        sql << ')';
      sql << " AS " << query;

      log_debug("PQexec(" << getPGConn() << ", \"" << sql.str() << "\")");
      PGresult* result = PQexec(getPGConn(), sql.str().c_str());

      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(sql.str(), "PQexec", result, true);
      }
#endif

      stmtName = s.str();

      log_debug("PQclear(" << result << ')');
      PQclear(result);
    }

    PGresult* Statement::execPrepared()
    {
      if (stmtName.empty())
        doPrepare();

      log_debug("PQexecPrepared(" << getPGConn() << ", \"" << stmtName
        << "\", " << values.size() << ", paramValues, paramLengths, 0, 0)");
      PGresult* result = PQexecPrepared(getPGConn(), stmtName.c_str(),
        getNParams(), getParamValues(), getParamLengths(), 0, 0);

      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(query, "PQexecPrepared", result, true);
      }

      return result;
    }

    template <typename T>
    void Statement::setValue(const std::string& col, T data)
    {
      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it == hostvarMap.end())
        log_warn("hostvariable :" << col << " not found");
      else
      {
        std::ostringstream v;
        v << data;
        values[it->second].setValue(v.str());
      }
    }

    template <typename T>
    void Statement::setStringValue(const std::string& col, T data)
    {
      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it == hostvarMap.end())
        log_warn("hostvariable :" << col << " not found");
      else
        values[it->second].setValue(data);
    }

    template <typename T>
    void Statement::setIsoValue(const std::string& col, T data)
    {
      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it == hostvarMap.end())
        log_warn("hostvariable :" << col << " not found");
      else
        values[it->second].setValue(data.getIso());
    }

#ifndef HAVE_PQPREPARE
    void Statement::setType(const std::string& col, const std::string& type)
    {
      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it != hostvarMap.end())
        values[it->second].setType(type);
    }
#endif

    void Statement::clear()
    {
      log_debug("clear()");
      for (valuesType::iterator it = values.begin(); it != values.end(); ++it)
        it->setNull();
    }

    void Statement::setNull(const std::string& col)
    {
      log_debug("setNull(\"" << col << "\")");

      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it == hostvarMap.end())
        log_warn("hostvariable :" << col << " not found");
      else
        values[it->second].setNull();
    }

    void Statement::setBool(const std::string& col, bool data)
    {
      log_debug("setBool(\"" << col << "\", " << data << ')');

      hostvarMapType::const_iterator it = hostvarMap.find(col);
      if (it == hostvarMap.end())
        log_warn("hostvariable :" << col << " not found");
      else
        values[it->second].setValue(data ? "T" : "F");

      SET_TYPE(col, "bool");
    }

    void Statement::setInt(const std::string& col, int data)
    {
      log_debug("setInt(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "int");
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
      log_debug("setUnsigned(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "int");
    }

    void Statement::setInt32(const std::string& col, int32_t data)
    {
      log_debug("setInt32(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "int");
    }

    void Statement::setUnsigned32(const std::string& col, uint32_t data)
    {
      log_debug("setUnsigned32(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "int");
    }
    
    void Statement::setInt64(const std::string& col, int64_t data)
    {
      log_debug("setInt64(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "bigint");
    }

    void Statement::setUnsigned64(const std::string& col, uint64_t data)
    {
      log_debug("setUnsigned64(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "bigint");
    }

    void Statement::setDecimal(const std::string& col, const Decimal& data)
    {
      log_debug("setDecimal(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "numeric");
    }
    
    void Statement::setFloat(const std::string& col, float data)
    {
      log_debug("setFloat(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "numeric");
    }

    void Statement::setDouble(const std::string& col, double data)
    {
      log_debug("setDouble(\"" << col << "\", " << data << ')');
      setValue(col, data);
      SET_TYPE(col, "numeric");
    }

    void Statement::setChar(const std::string& col, char data)
    {
      log_debug("setChar(\"" << col << "\", '" << data << "')");
      setStringValue(col, std::string(1, data));
      SET_TYPE(col, "text");
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
      log_debug("setString(\"" << col << "\", \"" << data << "\")");
      setStringValue(col, data);
      SET_TYPE(col, "text");
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
      log_debug("setDate(\"" << col << "\", " << data.getIso() << ')');
      setIsoValue(col, data);
      SET_TYPE(col, "date");
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
      log_debug("setTime(\"" << col << "\", " << data.getIso() << ')');
      setIsoValue(col, data);
      SET_TYPE(col, "time");
    }

    void Statement::setDatetime(const std::string& col, const Datetime& data)
    {
      log_debug("setDatetime(\"" << col << "\", " << data.getIso() << ')');
      setIsoValue(col, data);
      SET_TYPE(col, "datetime");
    }

    Statement::size_type Statement::execute()
    {
      log_debug("execute()");

      PGresult* result = execPrepared();

      std::istringstream tuples(PQcmdTuples(result));
      unsigned ret = 0;
      tuples >> ret;

      log_debug("PQclear(" << result << ')');
      PQclear(result);

      return ret;
    }

    tntdb::Result Statement::select()
    {
      log_debug("select()");
      PGresult* result = execPrepared();
      return tntdb::Result(new Result(tntdb::Connection(conn), result));
    }

    tntdb::Row Statement::selectRow()
    {
      tntdb::Result result = select();

      if (result.size() <= 0)
        throw NotFound();

      return result[0];
    }

    tntdb::Value Statement::selectValue()
    {
      tntdb::Result result = select();

      if (result.size() <= 0)
        throw NotFound();

      return result[0][0];
    }

    ICursor* Statement::createCursor(unsigned fetchsize)
    {
      return new Cursor(this, fetchsize);
    }

    const char* const* Statement::getParamValues()
    {
      for (unsigned n = 0; n < values.size(); ++n)
        paramValues[n] = values[n].getValue();
      return paramValues.data();
    }

    const int* Statement::getParamLengths()
    {
      for (unsigned n = 0; n < values.size(); ++n)
        paramLengths[n] = values[n].getLength();
      return paramLengths.data();
    }

    PGconn* Statement::getPGConn()
    {
      return conn->getPGConn();
    }
  }
}
