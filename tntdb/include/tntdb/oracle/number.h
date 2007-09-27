/* 
 * Copyright (C) 2007 Tommi Maekitalo, Mark Wright
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
      unsigned char vnum[OCI_NUMBER_SIZE];
      char pad[2];
      sb1 scale;
    public:
      Number();
      Number(const Decimal &decimal);
      Decimal getDecimal() const;
      unsigned char* getHandle() { return vnum; }
      sb1 *getScaleHandle() { return &scale; }
    };
  };
};

#endif
