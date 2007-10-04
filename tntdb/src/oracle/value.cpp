/* 
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <tntdb/oracle/value.h>
#include <tntdb/oracle/statement.h>
#include <tntdb/error.h>
#include <tntdb/decimal.h>
#include <sstream>
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

      /* retrieve the data type attribute */
      ret = OCIAttrGet(paramp, OCI_DTYPE_PARAM, &type, 0, OCI_ATTR_DATA_TYPE,
        stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_DATA_TYPE)");

      ret = OCIAttrGet(paramp, OCI_DTYPE_PARAM, &len, 0, OCI_ATTR_DATA_SIZE,
        stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_DATA_SIZE)");

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

        default:
          log_debug("OCIDefineByPos(SQLT_AVC)");
          data.reserve(len + 16);
          ret = OCIDefineByPos(stmt->getHandle(), &defp,
            stmt->getErrorHandle(), pos + 1, data.data(),
            len + 16, SQLT_AVC, &nullind, &len, 0, OCI_DEFAULT);
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
          return getValue<int>(data.data(), "int");
      }
    }

    int32_t Value::getInt32() const
    {
      return getInt();
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
          return getValue<unsigned>(data.data(), "unsigned");
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
          return getValue<int64_t>(data.data(), "int");
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
          return getValue<uint64_t>(data.data(), "int");
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
          return getValue<Decimal>(data.data(), "Decimal");
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
          return getValue<float>(data.data(), "float");
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
          return getValue<double>(data.data(), "double");
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
          return *data.data();
      }
    }

    void Value::getString(std::string& ret) const
    {
      if (isNull())
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
          ret = data.data();
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
