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

#include <tntdb/mysql/impl/statement.h>
#include <tntdb/mysql/impl/rowcontainer.h>
#include <tntdb/mysql/impl/boundrow.h>
#include <tntdb/mysql/impl/boundvalue.h>
#include <tntdb/mysql/impl/cursor.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/mysql/error.h>
#include <tntdb/stmtparser.h>
#include <sstream>
#include <cxxtools/log.h>

log_define("tntdb.mysql.statement")

namespace tntdb
{
namespace mysql
{
typedef std::multimap<std::string, unsigned> hostvarMapType;

namespace
{
    class SE : public StmtEvent
    {
        hostvarMapType& hostvarMap;
        unsigned idx;
        static const std::string hostvarInd;

    public:
        SE(hostvarMapType& hostvarMap_)
          : hostvarMap(hostvarMap_),
            idx(0)
          { }
        std::string onHostVar(const std::string& name);
        unsigned getCount() const  { return idx; }
    };

    std::string SE::onHostVar(const std::string& name)
    {
        log_debug("hostvar :" << name << ", idx=" << idx);
        hostvarMap.insert(hostvarMapType::value_type(name, idx++));
        return hostvarInd;
    }

    const std::string SE::hostvarInd = "?";
}

std::shared_ptr<BoundRow> Statement::getRow()
{
    if (rowPtr && rowPtr.unique())
    {
        rowPtr->clear();
        return rowPtr;
    }

    getFields();

    rowPtr.reset(new BoundRow(field_count));

    for (unsigned n = 0; n < field_count; ++n)
    {
        if (fields[n].length > 0x10000)
          // do not allocate buffers > 64k - use mysql_stmt_fetch_column instead later
          fields[n].length = 0x10000;

        rowPtr->initOutBuffer(n, fields[n]);
    }

    return rowPtr;
}

std::shared_ptr<BoundRow> Statement::fetchRow()
{
    std::shared_ptr<BoundRow> ptr = getRow();

    log_debug("mysql_stmt_bind_result(" << stmt << ", " << ptr->getMysqlBind() << ')');
    if (mysql_stmt_bind_result(stmt, ptr->getMysqlBind()) != 0)
        throw MysqlStmtError("mysql_stmt_bind_result", stmt);

    log_debug("mysql_stmt_fetch(" << stmt << ')');
    int ret = mysql_stmt_fetch(stmt);

    if (ret == MYSQL_DATA_TRUNCATED)
    {
        // fetch column data where truncated
        for (unsigned n = 0; n < field_count; ++n)
        {
            if (*ptr->getMysqlBind()[n].length > ptr->getMysqlBind()[n].buffer_length)
            {
                // actual length was longer than buffer_length, so this column is truncated
                fields[n].length = *ptr->getMysqlBind()[n].length;
                ptr->initOutBuffer(n, fields[n]);

                log_debug("mysql_stmt_fetch_column(" << stmt << ", BIND, " << n
                    << ", 0) with " << fields[n].length << " bytes");
                if (mysql_stmt_fetch_column(stmt, ptr->getMysqlBind() + n, n, 0) != 0)
                  throw MysqlStmtError("mysql_stmt_fetch_column", stmt);
            }
        }
    }
    else if (ret == MYSQL_NO_DATA)
        ptr = nullptr;
    else if (ret == 1)
        throw MysqlStmtError("mysql_stmt_fetch", stmt);

    return ptr;
}

Statement::Statement(Connection& conn_, MYSQL* mysql_,
  const std::string& query_)
  : conn(conn_),
    mysql(mysql_),
    stmt(0),
    fields(0),
    field_count(0)
{
    // parse hostvars
    StmtParser parser;
    SE se(hostvarMap);
    parser.parse(query_, se);

    log_debug("sql=\"" << parser.getSql() << "\" invars " << se.getCount());

    query = parser.getSql();
    inVars.setSize(se.getCount());
}

Statement::~Statement()
{
    if (stmt)
    {
        log_debug("mysql_stmt_close(" << stmt << ')');
        ::mysql_stmt_close(stmt);
    }
}

void Statement::clear()
{
    log_debug("statement " << stmt << " clear()");
    for (hostvarMapType::const_iterator it = hostvarMap.begin();
         it != hostvarMap.end(); ++it)
        inVars.setNull(it->second);
}

void Statement::setNull(const std::string& col)
{
    log_debug("statement " << stmt << " setNull(\"" << col << "\")");

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setNull(it->second);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setBool(const std::string& col, bool data)
{
    log_debug("statement " << stmt << " setBool(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setBool(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setShort(const std::string& col, short data)
{
    log_debug("statement " << stmt << " setShort(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setShort(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setInt(const std::string& col, int data)
{
    log_debug("statement " << stmt << " setInt(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setInt(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setLong(const std::string& col, long data)
{
    log_debug("statement " << stmt << " setLong(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setLong(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setUnsignedShort(const std::string& col, unsigned short data)
{
    log_debug("statement " << stmt << " setUnsignedShort(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setUnsignedShort(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setUnsigned(const std::string& col, unsigned data)
{
    log_debug("statement " << stmt << " setUnsigned(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setUnsigned(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setUnsignedLong(const std::string& col, unsigned long data)
{
    log_debug("statement " << stmt << " setUnsignedLong(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setUnsignedLong(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setInt32(const std::string& col, int32_t data)
{
    log_debug("statement " << stmt << " setInt32(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setInt32(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setUnsigned32(const std::string& col, uint32_t data)
{
    log_debug("statement " << stmt << " setUnsigned32(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setUnsigned32(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setInt64(const std::string& col, int64_t data)
{
    log_debug("statement " << stmt << " setInt64(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setInt64(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setUnsigned64(const std::string& col, uint64_t data)
{
    log_debug("statement " << stmt << " setUnsigned64(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setUnsigned64(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setDecimal(const std::string& col, const Decimal& data)
{
    log_debug("statement " << stmt << " setDecimal(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setDecimal(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setFloat(const std::string& col, float data)
{
    log_debug("statement " << stmt << " setFloat(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setFloat(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setDouble(const std::string& col, double data)
{
    log_debug("statement " << stmt << " setDouble(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setDouble(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setChar(const std::string& col, char data)
{
    log_debug("statement " << stmt << " setChar(\"" << col << "\", " << data << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setChar(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setString(const std::string& col, const std::string& data)
{
    log_debug("statement " << stmt << " setString(\"" << col << "\", \"" << data << "\")");

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setString(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setBlob(const std::string& col, const Blob& data)
{
    log_debug("statement " << stmt << " setBlob(\"" << col << "\", data {" << data.size() << "})");

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setBlob(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setDate(const std::string& col, const Date& data)
{
    log_debug("statement " << stmt << " setDate(\"" << col << "\", "
      << data.getIso() << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setDate(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setTime(const std::string& col, const Time& data)
{
    log_debug("statement " << stmt << " setTime(\"" << col << "\", "
      << data.getIso() << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setTime(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

void Statement::setDatetime(const std::string& col, const Datetime& data)
{
    log_debug("statement " << stmt << " setDatetime(\"" << col << "\", "
      << data.getIso() << ')');

    bool found = false;
    for (hostvarMapType::const_iterator it = hostvarMap.find(col);
         it != hostvarMap.end() && it->first == col; ++it)
    {
        found = true;
        inVars.setDatetime(it->second, data);
    }

    if (!found)
        log_warn("hostvar \"" << col << "\" not found");
}

Statement::size_type Statement::execute()
{
    log_debug("execute statement " << stmt);
    if (hostvarMap.empty())
    {
        return conn.execute(query);
    }
    else
    {
        // use statement-API
        stmt = getStmt();
        execute(stmt, 16);
        return mysql_stmt_affected_rows(stmt);
    }
}

tntdb::Result Statement::select()
{
    log_debug("select");

    if (hostvarMap.empty())
        return conn.select(query);

    if (fields)
        getRow();

    stmt = getStmt();
    execute(stmt, 16);

    if (mysql_stmt_store_result(stmt) != 0)
        throw MysqlStmtError("mysql_stmt_store_result", stmt);

    auto result = std::make_shared<RowContainer>();

    while (true)
    {
        auto ptr = fetchRow();
        if (!ptr)
            break;
        result->addRow(ptr);
    }

    return tntdb::Result(result);
}

std::shared_ptr<BoundRow> Statement::selectBoundRow()
{
    log_debug("selectRow");

    if (fields)
        getRow();

    stmt = getStmt();
    execute(stmt, 1);

    if (mysql_stmt_store_result(stmt) != 0)
        throw MysqlStmtError("mysql_stmt_store_result", stmt);

    auto ptr = fetchRow();

    if (!ptr)
        throw NotFound();

    return ptr;
}

tntdb::Row Statement::selectRow()
{
    if (hostvarMap.empty())
        return conn.selectRow(query);
    return Row(selectBoundRow());
}

tntdb::Value Statement::selectValue()
{
    log_debug("selectValue");
    if (hostvarMap.empty())
        return conn.selectValue(query);
    auto row = selectBoundRow();
    if (row->size() == 0)
        throw NotFound();

    return Value(std::make_shared<BoundValue>(row, row->getMysqlBind()[0]));
}

std::shared_ptr<ICursor> Statement::createCursor(unsigned fetchsize)
{
    return std::make_shared<Cursor>(*this, fetchsize);
}

MYSQL_STMT* Statement::createStmt()
{
    MYSQL_STMT* result = getStmt();
    stmt = 0;
    return result;
}

MYSQL_STMT* Statement::getStmt()
{
    if (stmt)
        return stmt;

    // initialize statement
    log_debug("mysql_stmt_init(" << mysql << ')');
    stmt = ::mysql_stmt_init(mysql);
    if (stmt == 0)
        throw MysqlError(mysql);
    log_debug("mysql_stmt_init(" << mysql << ") => " << stmt);

    log_debug("mysql_stmt_prepare(" << stmt << ", \"" << query << "\")");
    if (mysql_stmt_prepare(stmt, query.data(), query.size()) != 0)
    {
        // MysqlStmtError fetches the last error from MYSQL_STMT, so we need
        // to instantiate this before mysql_stmt_close
        MysqlStmtError e("mysql_stmt_prepare", stmt);
        log_debug("mysql_stmt_close(" << stmt << ')');
        ::mysql_stmt_close(stmt);
        stmt = 0;
        throw e;
    }

  /*
    // read always with cursor
    log_debug("mysql_stmt_attr_set(" << stmt << ", STMT_ATTR_CURSOR_TYPE)");
    unsigned long cursorType = CURSOR_TYPE_READ_ONLY;
    if (mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, &cursorType) != 0)
    {
        log_debug("error");

        MysqlStmtError e("mysql_stmt_attr_set", stmt);
        log_debug("mysql_stmt_close(" << stmt << ')');
        ::mysql_stmt_close(stmt);
        throw e;
    }
  */

    // check parametercount
    log_debug("mysql_stmt_param_count(" << stmt << ')');
    unsigned param_count = mysql_stmt_param_count(stmt);
    if (param_count != inVars.getSize())
    {
        log_debug("mysql_stmt_close(" << stmt << ')');
        ::mysql_stmt_close(stmt);
        stmt = 0;
        std::ostringstream msg;
        msg << "invalid parametercount in query; "
            << inVars.getSize() << " expected "
            << param_count << " found by MYSQL";
        throw std::runtime_error(msg.str());
    }

    log_debug("statement initialized " << stmt);
    return stmt;
}

void Statement::execute(MYSQL_STMT* stmt, unsigned fetchsize)
{
    // fetch multiple rows at once
    log_debug("mysql_stmt_attr_set(STMT_ATTR_PREFETCH_ROWS, " << fetchsize << ")");
    unsigned long count = fetchsize;
    if (mysql_stmt_attr_set(stmt, STMT_ATTR_PREFETCH_ROWS, &count) != 0)
        throw MysqlStmtError("mysql_stmt_attr_set", stmt);

    // bind parameters
    log_debug("mysql_stmt_bind_param(" << stmt << ')');
    if (mysql_stmt_bind_param(stmt, inVars.getMysqlBind()) != 0)
        throw MysqlStmtError("mysql_stmt_bind_param", stmt);

    log_debug("mysql_stmt_execute(" << stmt << ')');
    if (mysql_stmt_execute(stmt) != 0)
        throw MysqlStmtError("mysql_stmt_execute", stmt);
}

MYSQL_FIELD* Statement::getFields()
{
    if (fields == 0)
    {
        stmt = getStmt();

        log_debug("mysql_stmt_result_metadata(" << stmt << ')');
        MYSQL_RES* metadata = mysql_stmt_result_metadata(stmt);
        if (!metadata)
            throw Error("no metadata avaliable");
        log_debug("mysql_stmt_result_metadata(" << stmt << ") => " << metadata);

        log_debug("mysql_fetch_fields(" << metadata << ')');
        fields = mysql_fetch_fields(metadata);

        log_debug("mysql_num_fields(" << metadata << ')');
        field_count = mysql_num_fields(metadata);

        log_debug("mysql_free_result(" << metadata << ") (metadata)");
        ::mysql_free_result(metadata);
    }

    return fields;
}

unsigned Statement::getFieldCount()
{
    getFields();
    return field_count;
}

}
}
