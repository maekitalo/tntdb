/*
 * Copyright (C) 2007-2008 Tommi Maekitalo
 * Copyright (C) 2007-2008 Marc Boris Duerner
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

#include <tntdb/blob.h>

namespace tntdb
{
IBlob* BlobImpl::emptyInstance()
{
    class EmptyBlob : public IBlob
    {
    public:
        void assign(const char* data, std::size_t len) override
        {
            throw std::logic_error("cannot assign to empty blob");
        }

        char* reserve(std::size_t len, bool shrink)
        {
            throw std::logic_error("cannot resize empty blob");
        }

        IBlob* create() const
        {
            return new BlobImpl();
        }

        void destroy()
        { }
    };

    static EmptyBlob empty;
    return &empty;
}

IBlob::~IBlob()
{ }

void BlobImpl::assign(const char* data, std::size_t len)
{
    reserve(len, false);
    std::memcpy(_data, data, len);
    _size = len;
}

char* BlobImpl::reserve(std::size_t len, bool shrink)
{
    if (len == 0 && shrink)
    {
        delete[] _data;
        _data = 0;
        _size = 0;
    }
    else
    {
      if ((shrink && len != this->size()) || len > this->size())
      {
          delete[] _data;
          _data = new char[len];
      }
      _size = len;
    }

    return _data;
}

IBlob* BlobImpl::create() const
{ return new BlobImpl(); }

void BlobImpl::destroy()
{ delete this; }

}
