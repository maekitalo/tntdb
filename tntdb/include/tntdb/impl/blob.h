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

#ifndef TNTDB_IMPL_BLOB_H
#define TNTDB_IMPL_BLOB_H

#include <tntdb/iface/iblob.h>

namespace tntdb
{

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

        virtual void assign(const char* data, std::size_t len);

        virtual char* reserve(std::size_t len, bool shrink);

        virtual IBlob* create() const;

        virtual void destroy();

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


}

#endif // TNTDB_IMPL_BLOB_H

