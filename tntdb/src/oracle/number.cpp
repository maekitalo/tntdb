/*
 * Copyright (C) 2007 Tommi Maekitalo, Mark Wright
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

#include <tntdb/oracle/number.h>
#include <tntdb/oracle/error.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.number")

namespace tntdb
{
  namespace oracle
  {
    Number::Number()
    {
      memset(ociNumber.OCINumberPart, 0, OCI_NUMBER_SIZE);
    }

    void Number::setLong(long data, OCIError* errhp)
    {
      sword convRet = OCINumberFromInt(errhp,
        &data, sizeof(data), OCI_NUMBER_SIGNED,
        &ociNumber);

      error::checkError(errhp, convRet, "OCINumberFromInt");
    }

    void Number::setUnsignedLong(unsigned long data, OCIError* errhp)
    {
      sword convRet = OCINumberFromInt(errhp,
        &data, sizeof(data), OCI_NUMBER_UNSIGNED,
        &ociNumber);

      error::checkError(errhp, convRet, "OCINumberFromInt");
    }

    void Number::setInt64(int64_t data, OCIError* errhp)
    {
      sword convRet = OCINumberFromInt(errhp,
        &data, sizeof(data), OCI_NUMBER_SIGNED,
        &ociNumber);

      error::checkError(errhp, convRet, "OCINumberFromInt");
    }

    void Number::setUnsigned64(uint64_t data, OCIError* errhp)
    {
      sword convRet = OCINumberFromInt(errhp,
        &data, sizeof(data), OCI_NUMBER_UNSIGNED,
        &ociNumber);

      error::checkError(errhp, convRet, "OCINumberFromInt");
    }

    void Number::setDecimal(const Decimal& decimal, OCIError* errhp)
    {
      std::string s = decimal.toStringSci();
      static const text fmt[] = "9.99999999999999999999999999999999999999EEEE";

      log_debug("OCINumberFromText(\"" << s << "\")");
      sword convRet = OCINumberFromText( errhp,
        reinterpret_cast<const text*>(s.data()), s.size(),
        reinterpret_cast<const text*>(fmt), sizeof(fmt) - 1,
        reinterpret_cast<const text*>(""), 0,
        &ociNumber);

      error::checkError(errhp, convRet, "OCINumberFromText");
    }

    Number::Number(const Decimal &decimal, OCIError* errhp)
    {
      setDecimal(decimal, errhp);
    }


    Decimal Number::getDecimal(OCIError* errhp) const
    {
      char buffer[64];
      ub4 bufsize = sizeof(buffer);

      log_debug("OCINumberToText");
      static const text fmt[] = "9.99999999999999999999999999999999999999EEEE";
      sword convRet = OCINumberToText(errhp,
        const_cast<OCINumber*>(&ociNumber), fmt, sizeof(fmt) - 1,
        reinterpret_cast<const text*>(""), 0,
        &bufsize, reinterpret_cast<text*>(buffer));

      error::checkError(errhp, convRet, "OCINumberToText");

      log_debug("OCINumberToText => \"" << buffer << '"');

      return Decimal(std::string(buffer));
    }
  };
};
