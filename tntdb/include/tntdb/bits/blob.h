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

#ifndef TNTDB_BITS_BLOB_H
#define TNTDB_BITS_BLOB_H

#include <tntdb/iface/iblob.h>
#include <tntdb/impl/blob.h>
#include <cxxtools/smartptr.h>

namespace tntdb
{

/** @brief Binary large objects
*/
class Blob
{
    //! @brief Release policy for SmartPtr
    template <typename T>
    struct Release
    {
        void destroy(IBlob* blob)
        { blob->destroy(); }
    };

    //! @brief Pointer to shared data
    cxxtools::SmartPtr< IBlob, cxxtools::InternalRefCounted, Release > m_data;

public:
    Blob()
    : m_data( BlobImpl::emptyInstance() )
    { }

    /** Construct a Blob with data of a given length

        Constructs a Blob using a default implementation using new/delete
        to manage the blob-data and the shared data object. The first \a len
        bytes of the data pointed to by \a data are copied to this Blob.
    */
    Blob(const char* data, size_t len)
    : m_data( new BlobImpl() )
    {
        m_data->assign(data, len);
    }

    /** Construct a Blob to use a customized implementation
    */
    explicit Blob(IBlob* b)
    : m_data(b)
    { }

    /** assigns the data to this blob object
     */
    void assign(const char* data, size_t len)
    {
        // copy-on-write
        if ( m_data->getRefs() > 1 )
        {
            m_data = m_data->create();
        }

        m_data->assign(data, len);
    }

    /** makes sure, the buffer it at least len bytes.
     *  If shrink is set, the buffer will be exactly len bytes.
     *  Data is not preserved when reallocated.
     */
    char* reserve(size_t len, bool shrink = false)
    {
        return m_data->reserve(len, shrink);
    }

    bool operator==(const Blob& b) const
    {
        return *m_data == *b.m_data;
    }

    bool operator!=(const Blob& b) const
    {
        return !operator==(b);
    }

    /** Returns a pointer to the data or 0 if no data is set.
    */
    const char* data() const
    {
        return  m_data->data();
    }

    /** Returns the size of the data
    */
    size_t size() const
    {
        return m_data->size();
    }

};

}

#endif // TNTDB_BITS_BLOB_H

