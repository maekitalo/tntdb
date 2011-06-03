/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <tntdb/oracle/statement.h>
#include <tntdb/oracle/connection.h>
#include <tntdb/oracle/result.h>
#include <tntdb/oracle/row.h>
#include <tntdb/oracle/value.h>
#include <tntdb/oracle/cursor.h>
#include <tntdb/bits/value.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/result.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.statement")

namespace tntdb
{
  namespace oracle
  {
    Statement::Bind* Statement::getBindPtr(const std::string& col)
    {
      BindMapType::iterator it = bindMap.find(col);
      if (it == bindMap.end())
        it = bindMap.insert(BindMapType::value_type(col, new Bind())).first;
      return it->second;
    }

    Statement::Statement(Connection* conn_, const std::string& query_)
      : conn(conn_),
        query(query_),
        stmtp(0)
    {
      getHandle();
    }

    Statement::~Statement()
    {
      if (stmtp)
      {
        log_debug("release statement handle " << stmtp);
        sword ret = OCIHandleFree(stmtp, OCI_HTYPE_STMT);
        if (ret != OCI_SUCCESS)
          log_error("OCIHandleFree(" << stmtp << " OCI_HTYPE_STMT) failed");
      }
    }

    // methods of IStatement

    void Statement::clear()
    {
      for (BindMapType::iterator it = bindMap.begin(); it != bindMap.end(); ++it)
      {
        it->second->setNull();

        log_debug("OCIBindByName, clear(\"" << it->first << "\")");
        sword ret = OCIBindByName(getHandle(), &it->second->ptr, conn->getErrorHandle(),
          0, 0, 0, 0,
          SQLT_INT, &it->second->indicator, 0, 0, 0, 0, OCI_DEFAULT);

        checkError(ret, "OCIBindByName");
      }
    }

    void Statement::setNull(const std::string& col)
    {
      Bind &b = getBind(col);
      b.setNull();

      log_debug("OCIBindByName, setNull(\"" << col << "\")");
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        0, 0,
        SQLT_CHR, &b.indicator, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setBool(const std::string& col, bool data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(bool));

      log_debug("OCIBindByName, setBool(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(bool),
        SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setShort(const std::string& col, short data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(int));

      log_debug("OCIBindByName, setInt(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(short),
        SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setInt(const std::string& col, int data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(int));

      log_debug("OCIBindByName, setInt(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(int),
        SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setLong(const std::string& col, long data)
    {
      Bind &b = getBind(col);
      b.number.setLong(data, conn->getErrorHandle());
      
      log_debug("OCIBindByName, setLong(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
        0, 0, 0, 0, 0, OCI_DEFAULT);
      
      checkError(ret, "OCIBindByName");
    }

    void Statement::setUnsignedShort(const std::string& col, unsigned short data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(unsigned short));

      log_debug("OCIBindByName, setUnsigned(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(unsigned short),
        SQLT_UIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(unsigned));

      log_debug("OCIBindByName, setUnsigned(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(unsigned),
        SQLT_UIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setUnsignedLong(const std::string& col, unsigned long data)
    {
      
      Bind &b = getBind(col);
      b.number.setUnsignedLong(data, conn->getErrorHandle());
      
      log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
        0, 0, 0, 0, 0, OCI_DEFAULT);
      
      checkError(ret, "OCIBindByName");
    }

    void Statement::setInt32(const std::string& col, int32_t data)
    {
      return setInt(col, data);
    }
    
    void Statement::setUnsigned32(const std::string& col, uint32_t data)
    {
      return setUnsigned(col, data);
    }

    void Statement::setInt64(const std::string& col, int64_t data)
    {
      Bind &b = getBind(col);
      b.number.setInt64(data, conn->getErrorHandle());
      
      log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
        0, 0, 0, 0, 0, OCI_DEFAULT);
      
      checkError(ret, "OCIBindByName");
    }


    void Statement::setUnsigned64(const std::string& col, uint64_t data)
    {
      Bind &b = getBind(col);
      b.number.setUnsigned64(data, conn->getErrorHandle());
      
      log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        (void *)b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
        0, 0, 0, 0, 0, OCI_DEFAULT);
      
      checkError(ret, "OCIBindByName");

    }
    
    void Statement::setDecimal(const std::string& col, const Decimal& decimal)
    {
      Bind &b = getBind(col);
      b.number.setDecimal(decimal, conn->getErrorHandle());
      
      log_debug("OCIBindByName, setDecimal(\"" << col << "\", " << decimal << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        b.number.getHandle(), OCI_NUMBER_SIZE,
        SQLT_VNU, 0, 0, 0, 0, 0, OCI_DEFAULT);
      
      checkError(ret, "OCIBindByName");
    }
    
    void Statement::setFloat(const std::string& col, float data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(float));

      log_debug("OCIBindByName, setFloat(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(float),
        SQLT_FLT, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setDouble(const std::string& col, double data)
    {
      Bind &b = getBind(col);
      b.setData(reinterpret_cast<char*>(&data), sizeof(double));

      log_debug("OCIBindByName, setDouble(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], sizeof(double),
        SQLT_FLT, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setChar(const std::string& col, char data)
    {
      Bind &b = getBind(col);
      b.setData(&data, 1);

      log_debug("OCIBindByName, setChar(\"" << col << "\", " << data << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], 1,
        SQLT_AFC, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
      Bind &b = getBind(col);
      b.setData(data);

      log_debug("OCIBindByName, setString(\"" << col << "\", \"" << data << "\")");
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], data.size(),
        SQLT_AFC, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setBlob(const std::string& col, const tntdb::Blob& data)
    {
      Bind &b = getBind(col);
      b.setData(data.data(), data.size());

      log_debug("OCIBindByName, setBlob(\"" << col << "\", data{" << data.size() << "})");
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.data[0], data.size(),
        SQLT_BIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
      Bind &b = getBind(col);
      b.datetime = Datetime(conn, data);

      log_debug("OCIBindByName, setDate(\"" << col << "\", " << data.getIso() << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.datetime.getHandle(), sizeof(OCIDateTime**),
        SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
      Bind &b = getBind(col);
      b.datetime = Datetime(conn, data);

      log_debug("OCIBindByName, setTime(\"" << col << "\", " << data.getIso() << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.datetime.getHandle(), sizeof(OCIDateTime**),
        SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    void Statement::setDatetime(const std::string& col, const tntdb::Datetime& data)
    {
      Bind &b = getBind(col);
      b.datetime = Datetime(conn, data);

      log_debug("OCIBindByName, setDatetime(\"" << col << "\", " << data.getIso() << ')');
      sword ret = OCIBindByName(getHandle(), &b.ptr, conn->getErrorHandle(),
        reinterpret_cast<const text*>(col.data()), col.size(),
        &b.datetime.getHandle(), sizeof(OCIDateTime**),
        SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

      checkError(ret, "OCIBindByName");
    }

    Statement::size_type Statement::execute()
    {
      log_debug("execute statement " << getHandle() << " mode=" << (conn->isTransactionActive() ? "OCI_DEFAULT" : "OCI_COMMIT_ON_SUCCESS"));
      sword ret = OCIStmtExecute(conn->getSvcCtxHandle(), getHandle(), conn->getErrorHandle(),
        1, 0, 0, 0, conn->isTransactionActive() ? OCI_DEFAULT : OCI_COMMIT_ON_SUCCESS);
      checkError(ret, "OCIStmtExecute");

      ub4 rowcnt = 0;
      ret = OCIAttrGet(getHandle(), OCI_HTYPE_STMT, &rowcnt, 0, OCI_ATTR_ROW_COUNT,
        conn->getErrorHandle());
      checkError(ret, "OCIAttrGet");

      return rowcnt;
    }

    tntdb::Result Statement::select()
    {
      return tntdb::Result(new Result(this));
    }

    tntdb::Row Statement::selectRow()
    {
      sword ret = OCIStmtExecute(getConnection()->getSvcCtxHandle(),
        getHandle(), getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
      checkError(ret, "OCIStmtExecute");

      tntdb::Row row(new oracle::Row(this));
      ret = OCIStmtFetch(getHandle(), getErrorHandle(), 1, OCI_FETCH_NEXT,
        OCI_DEFAULT);
      checkError(ret, "OCIStmtFetch");

      return row;
    }

    tntdb::Value Statement::selectValue()
    {
      sword ret = OCIStmtExecute(getConnection()->getSvcCtxHandle(),
        getHandle(), getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
      checkError(ret, "OCIStmtExecute");

      tntdb::Value value(new oracle::Value(this, 0));
      ret = OCIStmtFetch(getHandle(), getErrorHandle(), 1, OCI_FETCH_NEXT,
        OCI_DEFAULT);
      checkError(ret, "OCIStmtFetch");

      return value;
    }

    ICursor* Statement::createCursor(unsigned fetchsize)
    {
      return new Cursor(this, fetchsize);
    }

    OCIStmt* Statement::getHandle()
    {
      if (stmtp == 0)
      {
        log_debug("prepare statement \"" << query << '"');

        sword ret = OCIHandleAlloc(conn->getEnvHandle(), (void**)&stmtp, OCI_HTYPE_STMT, 0, 0);
        checkError(ret, "OCIHandleAlloc(OCI_HTYPE_STMT)");

        ret = OCIStmtPrepare(stmtp, conn->getErrorHandle(),
          reinterpret_cast<const unsigned char*>(query.data()),
          query.size(), OCI_NTV_SYNTAX, OCI_DEFAULT);
        checkError(ret, "OCIStmtPrepare");

        log_debug("statement handle " << stmtp);
      }

      return stmtp;
    }
  }
}
