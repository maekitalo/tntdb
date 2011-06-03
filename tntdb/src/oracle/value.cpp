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

#include <tntdb/oracle/value.h>
#include <tntdb/oracle/statement.h>
#include <tntdb/error.h>
#include <tntdb/decimal.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cxxtools/log.h>

log_define("tntdb.oracle.value")

namespace tntdb
{
  namespace oracle
  {
    namespace
    {
      template <typename T>
      T getValue(const std::string& s, const char* tname)
      {
        std::istringstream in(s);
        T ret;
        in >> ret;
        if (!in)
        {
          std::ostringstream msg;
          msg << "can't convert \"" << s << "\" to " << tname;
          throw TypeError(msg.str());
        }
        return ret;
      }

      template <typename T, typename I>
      T getValueFloat(I from, I to, const char* tname)
      {
        // This is a stupid workaround for converting decimal comma into decimal point
        // It should be actually done in OCI
        std::string o;
        o.reserve(to - from);
        std::replace_copy(from, to, std::back_inserter(o), ',', '.');
        return getValue<T>(o, tname);
      }

      template <typename T>
      std::string toString(T v)
      {
        std::ostringstream ret;
        ret << v;
        return ret.str();
      }

      template <>
      std::string toString(float v)
      {
        std::ostringstream ret;
        ret.precision(24);
        ret << v;
        return ret.str();
      }
      
      template <>
      std::string toString(double v)
      {
        std::ostringstream ret;
        ret.precision(24);
        ret << v;
        return ret.str();
      }
      
      template <>
      std::string toString(Decimal v)
      {
        std::ostringstream ret;
        ret.precision(24);
        ret << v;
        return ret.str();
      }
    }

    Value::Value(Statement* stmt, ub4 pos)
    {
      sword ret;

      /* get parameter-info */
      ret = OCIParamGet(stmt->getHandle(), OCI_HTYPE_STMT,
        stmt->getErrorHandle(),
        reinterpret_cast<void**>(&paramp), pos + 1);
      stmt->checkError(ret, "OCIParamGet");

      init(stmt, paramp, pos);

      OCIDescriptorFree(paramp, OCI_DTYPE_PARAM);
      paramp = 0;
    }

    Value::Value(Statement* stmt, OCIParam* paramp, ub4 pos)
    {
      init(stmt, paramp, pos);
    }

    void Value::init(Statement* stmt, OCIParam* paramp, ub4 pos)
    {
      sword ret;

      /* retrieve column name */
      ub4 col_name_len = 0;
      text* col_name = 0;
      ret = OCIAttrGet(paramp, OCI_DTYPE_PARAM, &col_name, &col_name_len,
        OCI_ATTR_NAME, stmt->getErrorHandle());

      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_NAME)");
      colName.assign(reinterpret_cast<const char*>(col_name), col_name_len);

      /* retrieve the data type attribute */
      ret = OCIAttrGet(paramp, OCI_DTYPE_PARAM, &type, 0, OCI_ATTR_DATA_TYPE,
        stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_DATA_TYPE)");

      ret = OCIAttrGet(paramp, OCI_DTYPE_PARAM, &len, 0, OCI_ATTR_DATA_SIZE,
        stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_DATA_SIZE)");

      log_debug("column name=\"" << colName << "\" type=" << type << " size=" << len);

      /* define outputvariable */
      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          log_debug("OCIDefineByPos(SQLT_TIMESTAMP)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &datetime.getReference(stmt->getConnection()),
            sizeof(OCIDateTime*), SQLT_TIMESTAMP, &nullind, &len, 0, OCI_DEFAULT);
          break;

        case SQLT_INT:
          log_debug("OCIDefineByPos(SQLT_INT)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &longValue,
            sizeof(longValue), SQLT_INT, &nullind, &len, 0, OCI_DEFAULT);
          break;

        case SQLT_UIN:
          log_debug("OCIDefineByPos(SQLT_UIN)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &uint32Value,
            sizeof(uint32Value), SQLT_UIN, &nullind, &len, 0, OCI_DEFAULT);
          break;

        case SQLT_FLT:
          log_debug("OCIDefineByPos(SQLT_FLT)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &doubleValue,
            sizeof(doubleValue), SQLT_FLT, &nullind, &len, 0, OCI_DEFAULT);
          break;

        case SQLT_NUM:
        case SQLT_VNU:
          log_debug("OCIDefineByPos(SQLT_VNU)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, number.getHandle(),
            OCI_NUMBER_SIZE, SQLT_VNU, &nullind, &len, 0, OCI_DEFAULT);
          break;

        case SQLT_BLOB:
          log_debug("OCIDefineByPos(SQLT_LOB)");
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &blob.getHandle(stmt->getConnection()),
            0, SQLT_BLOB, &nullind, &len, 0, OCI_DEFAULT);
          break;

        default:
          log_debug("OCIDefineByPos(SQLT_AFC)");
          data.resize(len + 16);
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, &data[0],
            len + 16, SQLT_AFC, &nullind, &len, 0, OCI_DEFAULT);
          break;
      }
      stmt->checkError(ret, "OCIDefineByPos");
    }

    Value::~Value()
    {
      if (defp)
        OCIHandleFree(defp, OCI_HTYPE_DEFINE);
      if (paramp)
        OCIDescriptorFree(paramp, OCI_DTYPE_PARAM);
    }

    bool Value::isNull() const
    {
      return nullind != 0;
    }

    bool Value::getBool() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
          return longValue != 0;

        case SQLT_UIN:
          return uint32Value != 0;

        case SQLT_FLT:
          return doubleValue != 0;

        case SQLT_NUM:
        case SQLT_VNU:
          return !number.getDecimal().isZero();
          
        default:
          return data[0] == 't' || data[0] == 'T'
              || data[0] == 'y' || data[0] == 'Y'
              || data[0] == '1';

      }
    }

    short Value::getShort() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<short>(longValue);

        case SQLT_FLT:
          return static_cast<short>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getShort();
          
        default:
          return getValue<short>(std::string(&data[0], len), "int");
      }
    }

    int Value::getInt() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<int>(longValue);

        case SQLT_FLT:
          return static_cast<int>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getInt();
          
        default:
          return getValue<int>(std::string(&data[0], len), "int");
      }
    }

    long Value::getLong() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<long>(longValue);

        case SQLT_FLT:
          return static_cast<long>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getLong();
          
        default:
          return getValue<long>(std::string(&data[0], len), "long");
      }
    }

    int32_t Value::getInt32() const
    {
      return getInt();
    }

    unsigned short Value::getUnsignedShort() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<unsigned short>(longValue);

        case SQLT_FLT:
          return static_cast<unsigned short>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getUnsignedShort();
          
        default:
          return getValue<unsigned>(std::string(&data[0], len), "unsigned");
      }
    }

    unsigned Value::getUnsigned() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<unsigned>(longValue);

        case SQLT_FLT:
          return static_cast<unsigned>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getUnsigned();
          
        default:
          return getValue<unsigned>(std::string(&data[0], len), "unsigned");
      }
    }

    unsigned long Value::getUnsignedLong() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<unsigned long>(longValue);

        case SQLT_FLT:
          return static_cast<unsigned long>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getUnsignedLong();
          
        default:
          return getValue<unsigned long>(std::string(&data[0], len), "unsigned long");
      }
    }

    uint32_t Value::getUnsigned32() const
    {
      return getUnsigned();
    }

    int64_t Value::getInt64() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<int64_t>(longValue);

        case SQLT_FLT:
          return static_cast<int64_t>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getInt64();
          
        default:
          return getValue<int64_t>(std::string(&data[0], len), "int64_t");
      }
    }

    uint64_t Value::getUnsigned64() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<uint64_t>(longValue);

        case SQLT_FLT:
          return static_cast<uint64_t>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getUnsigned64();
          
        default:
          return getValue<uint64_t>(std::string(&data[0], len), "uint64_t");
      }
    }

    Decimal Value::getDecimal() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
        {
          Decimal decimal;
          decimal.setInt(longValue);
          return decimal;
        }

        case SQLT_FLT:
        {
          Decimal decimal;
          decimal.setDouble(doubleValue);
          return decimal;
        }

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal();
          
        default:
          return getValue<Decimal>(std::string(&data[0], len), "Decimal");
      }
    }
    
    float Value::getFloat() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<float>(longValue);

        case SQLT_FLT:
          return static_cast<float>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getFloat();

        default:
          return getValueFloat<float>(data.begin(), data.begin() + len, "float");
      }
    }

    double Value::getDouble() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<double>(longValue);

        case SQLT_FLT:
          return static_cast<double>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return number.getDecimal().getDouble();
          
        default:
          return getValueFloat<double>(data.begin(), data.begin() + len, "double");
      }
    }

    char Value::getChar() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          throw TypeError();

        case SQLT_INT:
        case SQLT_UIN:
          return static_cast<char>(longValue);

        case SQLT_FLT:
          return static_cast<char>(doubleValue);

        case SQLT_NUM:
        case SQLT_VNU:
          return static_cast<char>(number.getDecimal().getInt());
          
        default:
          return data[0];
      }
    }

    void Value::getString(std::string& ret) const
    {
      log_debug("Value::getString with type=" << type << " name=" << colName
        << " size=" << len);

      if (type != SQLT_AFC && type != SQLT_CHR && isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          ret = datetime.getDatetime().getIso();
          break;

        case SQLT_INT:
        case SQLT_UIN:
          ret = toString(longValue);
          break;

        case SQLT_FLT:
          ret = toString(doubleValue);
          break;

        case SQLT_NUM:
        case SQLT_VNU:
          ret = number.getDecimal().toString();
          break;

        default:
          ret.assign(&data[0], len > 0 ? len : 0);
      }
    }

    void Value::getBlob(tntdb::Blob& ret) const
    {
      log_debug("get blob from type " << type);

      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
        case SQLT_INT:
        case SQLT_UIN:
        case SQLT_FLT:
        case SQLT_NUM:
        case SQLT_VNU:
          throw TypeError();

        case SQLT_BLOB:
          blob.getData(ret);
          break;

        default:
          ret.assign(&data[0], len > 0 ? len - 1 : 0);
      }
    }

    Date Value::getDate() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          return datetime.getDate();

        default:
          throw TypeError();
      }
    }

    Time Value::getTime() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          return datetime.getTime();

        default:
          throw TypeError();
      }
    }

    tntdb::Datetime Value::getDatetime() const
    {
      if (isNull())
        throw NullValue();

      switch (type)
      {
        case SQLT_DAT:
        case SQLT_TIMESTAMP:
        case SQLT_TIMESTAMP_TZ:
        case SQLT_TIMESTAMP_LTZ:
          return datetime.getDatetime();

        default:
          throw TypeError();
      }
    }

  }
}
