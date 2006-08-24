/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNTDB_IFACE_ISTATEMENT_H
#define TNTDB_IFACE_ISTATEMENT_H

#include <tntdb/refcounted.h>
#include <tntdb/noncopyable.h>
#include <string>

namespace tntdb
{
  class Result;
  class Row;
  class Value;
  class Date;
  class Time;
  class Datetime;
  class ICursor;

  class IStatement : public RefCounted, private NonCopyable
  {
    public:
      typedef unsigned size_type;

      virtual void clear() = 0;

      virtual void setNull(const std::string& col) = 0;
      virtual void setBool(const std::string& col, bool data) = 0;
      virtual void setInt(const std::string& col, int data) = 0;
      virtual void setUnsigned(const std::string& col, unsigned data) = 0;
      virtual void setFloat(const std::string& col, float data) = 0;
      virtual void setDouble(const std::string& col, double data) = 0;
      virtual void setChar(const std::string& col, char data) = 0;
      virtual void setString(const std::string& col, const std::string& data) = 0;
      virtual void setDate(const std::string& col, const Date& data) = 0;
      virtual void setTime(const std::string& col, const Time& data) = 0;
      virtual void setDatetime(const std::string& col, const Datetime& data) = 0;

      virtual size_type execute() = 0;
      virtual Result select() = 0;
      virtual Row selectRow() = 0;
      virtual Value selectValue() = 0;
      virtual ICursor* createCursor() = 0;
  };
}

#endif // TNTDB_IFACE_ISTATEMENT_H

