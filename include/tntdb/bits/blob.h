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

#ifndef TNTDB_BITS_BLOB_H
#define TNTDB_BITS_BLOB_H

#include <tntdb/iface/iblob.h>
#include <tntdb/impl/blob.h>
#include <cxxtools/smartptr.h>

namespace tntdb
{
  /// @brief Binary large objects
  class Blob
  {
  public:
      /// @brief Release policy for SmartPtr
      template <typename T>
      struct Release
      {
        void destroy(IBlob* blob)
          { blob->destroy(); }
      };

  private:
      /// @brief Pointer to shared data
      cxxtools::SmartPtr<IBlob, cxxtools::InternalRefCounted, Release> _data;

  public:
      Blob()
        : _data(BlobImpl::emptyInstance())
        { }

      /** Construct a Blob with data of a given length

          Constructs a Blob using a default implementation using new/delete
          to manage the blob-data and the shared data object. The first \a len
          bytes of the data pointed to by \a data are copied to this Blob.
       */
      Blob(const char* data, std::size_t len)
        : _data(new BlobImpl())
        { _data->assign(data, len); }

      /// Construct a Blob to use a customized implementation
      explicit Blob(IBlob* b)
        : _data(b)
        { }

      /// Assigns the data to this blob object
      void assign(const char* data, std::size_t len)
      {
          // copy-on-write
          if (_data->refs() > 1)
            _data = _data->create();

          _data->assign(data, len);
      }

      /** Make sure the buffer is big enough to hold at least len bytes

           If shrink is set, the buffer will be exactly len bytes.
           Data is not preserved when reallocated.
       */
      char* reserve(std::size_t len, bool shrink = false)
      {
        if (_data.getPointer() == BlobImpl::emptyInstance())
          _data = _data->create();
        return _data->reserve(len, shrink);
      }

      bool operator==(const Blob& b) const
        { return *_data == *b._data; }

      bool operator!=(const Blob& b) const
        { return !operator==(b); }

      /// Get a pointer to the data or 0 if no data is set.
      const char* data() const
        { return  _data->data(); }

      /// Get the size of the data
      std::size_t size() const
        { return _data->size(); }
  };
}

#endif // TNTDB_BITS_BLOB_H

