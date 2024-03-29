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

#ifndef TNTDB_ORACLE_NUMBER_H
#define TNTDB_ORACLE_NUMBER_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <tntdb/decimal.h>
#include <oci.h>

namespace tntdb
{
namespace oracle
{
class Number
{
    OCINumber ociNumber;

public:
    Number();
    Number(const Decimal &decimal, OCIError* errhp);

    void setLong(long data, OCIError* errhp);
    void setUnsignedLong(unsigned long data, OCIError* errhp);

    void setInt64(int64_t data, OCIError* errhp);
    void setUnsigned64(uint64_t data, OCIError* errhp);

    void setDecimal(const Decimal &decimal, OCIError* errhp);
    static Decimal getDecimal(const OCINumber* handle, OCIError* errhp);
    Decimal getDecimal(OCIError* errhp) const   { return getDecimal(&ociNumber, errhp); }

    static float getFloat(const OCINumber* handle, OCIError* errhp);
    float getFloat(OCIError* errhp) const       { return getFloat(&ociNumber, errhp); }

    static double getDouble(const OCINumber* handle, OCIError* errhp);
    double getDouble(OCIError* errhp) const     { return getDouble(&ociNumber, errhp); }

    void* getHandle() { return &ociNumber; }
};
}
}

#endif
