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

#ifndef TNTDB_BLOB_H
#define TNTDB_BLOB_H

#include <string>
#include <cxxtools/smartptr.h>

namespace tntdb
{
  class Blob
  {
      cxxtools::SmartPtr<std::string, cxxtools::ExternalRefCounted> dataptr;

    public:
      Blob()  {}
      Blob(const char* data, unsigned len)
        : dataptr(new std::string(data, len))
        { }

      void assign(const char* data, unsigned len)
      {
        if (!dataptr.getPointer() || dataptr.getRefs() > 1)
          dataptr = new std::string(data, len);
        else
          dataptr->assign(data, len);
      }

      void assign(const std::string& str)
      {
        if (!dataptr.getPointer() || dataptr.getRefs() > 1)
          dataptr = new std::string(str);
        else
          dataptr->assign(str);
      }

      std::string& getStringRef()
      {
        if (!dataptr.getPointer() || dataptr.getRefs() > 1)
          dataptr = new std::string();
        return *dataptr;
      }

      const std::string& getString() const
      {
        if (!dataptr.getPointer())
          const_cast<Blob*>(this)->dataptr = new std::string();
        else if (dataptr.getRefs() > 1)
          const_cast<Blob*>(this)->dataptr = new std::string(*dataptr);
        return *dataptr;
      }

      bool operator== (const Blob& b) const
      {
        return dataptr.getPointer() ? b.dataptr.getPointer() ? *dataptr == *b.dataptr
                                                             : dataptr->size() == 0
                                    : b.dataptr->size() == 0;
      }

      bool operator!= (const Blob& b) const
      { return !operator== (b); }

      const char* data() const
      { dataptr.getPointer() ? dataptr->data() : 0; }

      unsigned size() const
      { dataptr.getPointer() ? dataptr->size() : 0; }

  };
}

#endif // TNTDB_BLOB_H

