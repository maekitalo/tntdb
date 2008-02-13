/* 
 * Copyright (C) 2007-2008 Tommi Maekitalo
 * Copyright (C) 2007-2008 Marc Boris Duerner
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

#include <tntdb/blob.h>

namespace tntdb
{
    IBlob::~IBlob()
    { }

    void BlobImpl::assign(const char* data, size_t len)
    {
        reserve(len, false);
        std::memcpy(_data, data, len);
        _size = len;
    }

    char* BlobImpl::reserve(size_t len, bool shrink)
    {
        if (len == 0 && shrink)
        {
            delete[] _data;
            _data = 0;
            _size = 0;
        }
        else
        {
          if (shrink && len != this->size() || len > this->size())
          {
              delete[] _data;
              _data = new char[len];
          }
          _size = len;
        }
    }

    IBlob* BlobImpl::create() const
    { return new BlobImpl(); }

    void BlobImpl::destroy()
    { delete this; }

}
