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
#include <string> // remove
#include <cstddef>
#include <cstring>
#include <cstdlib>

namespace tntdb
{

/** @brief Implementation base of Blob values

    This class serves as the base for the shared implementation classes used
    in Blob objects. The non-virtual interface consists of a pointer to the
    blob-data and its size. The implementor has to override three virtual methods,
    IBlob::assign, IBlob::create and IBlob::destroy. The main purpose of these
    methods is to customize memory allocation of blob-data, aswell as the shared
    implementation class derived from IBlob. A default implementation, called
    BlobData, is provided, that uses malloc()/free() to aquire space for the
    blob-data and new/delete for the implementation class.
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

        IBlob(size_t refs)
        : cxxtools::RefCounted(refs)
        , _data(0)
        , _size(0)
        { }

        char* _data;
        size_t _size;
};

/** @brief Default Blob value implementation

    This implementation uses new/delete to create and destroy
    the shared objects and malloc/free to allocate memory for
    the blob-data.
*/
class BlobData : public IBlob
{
    public:
        BlobData()
        { }

        ~BlobData()
        {
            if(_data)
                std::free(_data);
        }

        virtual void assign(const char* data, size_t len)
        {
            if( len > this->size() )
            {
                if(_data)
                    std::free(_data);

                _data = (char*)std::malloc(len);
            }

            std::memcpy(_data, data, len);
            _size = len;
        }

        virtual IBlob* create() const
        { return new BlobData(); }

        virtual void destroy()
        { delete this; }

        static BlobData* emptyInstance()
        {
            static BlobData empty(1);
            return &empty;
        }

    protected:
        BlobData(size_t refs)
        : IBlob(refs)
        {}
};


/** @internal Initialize statics in BlobData during static initialization

    Thread-safety.
*/
static struct BlobDataInitializer
{
    BlobDataInitializer()
    {
        BlobData::emptyInstance();
    }
} pt_blobdata_initializer;


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
    : m_data( BlobData::emptyInstance() )
    { }

    /** Construct a Blob with data of a given length

        Constructs a Blob using a default implementation using malloc/free
        to manage the blob-data and new/delete to manage the shared data
        object. The first \a len bytes of the data pointed to by \a data are
        copied to this Blob.
    */
    Blob(const char* data, size_t len)
    : m_data( new BlobData() )
    {
        m_data->assign(data, len);
    }

    /** Construct a Blob to use a customized implementation
    */
    Blob(IBlob* b)
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

    //! OBSOLETE, just to make it compile
    const std::string& getString() const
    {
        static std::string s;
        return s;
    }

    //! OBSOLETE, just to make it compile
    std::string& getStringRef()
    {
        static std::string s;
        return s;
    }

    bool operator==(const Blob& b) const
    {
        return *m_data == *m_data;
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

/*
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
      { return dataptr.getPointer() ? dataptr->data() : 0; }

      unsigned size() const
      { return dataptr.getPointer() ? dataptr->size() : 0; }

  };
*/
}

#endif // TNTDB_BLOB_H

