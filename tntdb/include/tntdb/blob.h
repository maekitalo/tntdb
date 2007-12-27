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

#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>
#include <cstddef>
#include <cstring>

namespace tntdb
{

/** @brief Implementation base of Blob values

    This class serves as the base for the shared implementation classes used
    in Blob objects. The non-virtual interface consists of a pointer to the
    blob-data and its size. The implementor has to override three virtual methods,
    IBlob::assign, IBlob::create and IBlob::destroy. The main purpose of these
    methods is to customize memory allocation of blob-data, aswell as the shared
    implementation class derived from IBlob. A default implementation, called
    BlobImpl, is provided, that uses new/delete and the implementation class.
*/
class IBlob : public cxxtools::RefCounted
{
    public:
        /** @brief Destructor
        */
        virtual ~IBlob() {}

        /** @brief Assign data of a given length

            The \a len bytes of the data pointed to by \a data are copied
            to this blob.
        */
        virtual void assign(const char* data, size_t len) = 0;

        /** @brief Create a value implementation

            Returns a pointer to a implementation class derived from IBlob.
            IBlob::destroy must be implemented accordingly to destroy this
            instance.
        */
        virtual IBlob* create() const = 0;

        /** @brief Destroy a value implementation

            Destroys an instance previously created by the IBlob::create.
        */
        virtual void destroy() = 0;

        /** @brief Returns the size of the blob-data.
        */
        size_t size() const
        { return _size; }

        /** @brief Returns a pointer to the blob-data or 0 if the blob is empty
        */
        const char* data() const
        { return _data; }

        /** qbrief Returns true if the two instances contain the same data
        */
        bool operator==(const IBlob& other) const
        {
            return _size == other._size &&
                ( std::strncmp(_data, other._data, _size) == 0 );
        }

    protected:
        IBlob()
        : _data(0)
        , _size(0)
        { }

        char* _data;
        size_t _size;
};

/** @brief Default Blob value implementation

    This implementation uses new/delete to create and destroy
    the shared objects and new/delete to allocate memory for
    the blob-data.
*/
class BlobImpl : public IBlob
{
    public:
        BlobImpl()
        { }

        ~BlobImpl()
        {
            delete[] _data;
        }

        virtual void assign(const char* data, size_t len)
        {
            if (len == 0)
            {
                delete[] _data;
                _data = 0;
                _size = 0;
                return;
            }

            if( len > this->size() )
            {
                delete[] _data;
                _data = new char[len];
            }

            std::memcpy(_data, data, len);
            _size = len;
        }

        virtual IBlob* create() const
        { return new BlobImpl(); }

        virtual void destroy()
        { delete this; }

        static BlobImpl* emptyInstance()
        {
            static BlobImpl empty(1);
            return &empty;
        }

    protected:
        // ctor, which constructs a instance, with a reference-counter of 1
        BlobImpl(int)
        { addRef(); }
};


/** @internal Initialize statics in BlobImpl during static initialization

    Thread-safety.
*/
static struct BlobDataInitializer
{
    BlobDataInitializer()
    {
        BlobImpl::emptyInstance();
    }
} tntdb_blobdata_initializer;


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

    void assign(const char* data, size_t len)
    {
        // copy-on-write
        if ( m_data->getRefs() > 1 )
        {
            m_data = m_data->create();
        }

        m_data->assign(data, len);
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

#endif // TNTDB_BLOB_H

