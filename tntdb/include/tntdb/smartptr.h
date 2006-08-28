/* 
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef TNTDB_SMARTPTR_H
#define TNTDB_SMARTPTR_H

namespace tntdb
{
  template <typename objectType>
  class RefLinked
  {
      mutable const RefLinked* prev;
      mutable const RefLinked* next;

    protected:
      void unlink(objectType* object)
      {
        if (object)
        {
          if (next == this)
          {
            delete object;
            object = 0;
          }
          else
          {
            next->prev = prev;
            prev->next = next;
          }
          next = prev = this;
        }
      }

      void link(const RefLinked& ptr, objectType* object)
      {
        prev = &ptr;
        next = ptr.next;
        prev->next = this;
        next->prev = this;
      }
  };

  template <typename objectType>
  class InternalRefCounted
  {
    protected:
      void unlink(objectType* object)
      {
        if (object)
          object->release();
      }

      void link(const InternalRefCounted& ptr, objectType* object)
      {
        if (object)
          object->addRef();
      }

  };

  template <typename objectType,
            template <class> class ownershipPolicy = InternalRefCounted>
  class SmartPtr : public ownershipPolicy<objectType>
  {
      objectType* object;
      typedef ownershipPolicy<objectType> ownershipPolicyType;

    public:
      SmartPtr()
        : object(0)
        {}
      SmartPtr(objectType* ptr)
        : object(ptr)
        { ownershipPolicyType::link(*this, ptr); }
      SmartPtr(const SmartPtr& ptr)
        : object(ptr.object)
        { ownershipPolicyType::link(ptr, ptr.object); }
      ~SmartPtr()
        { ownershipPolicyType::unlink(object); }

      SmartPtr& operator= (const SmartPtr& ptr)
      {
        if (object != ptr.object)
        {
          ownershipPolicyType::unlink(object);
          object = ptr.object;
          ownershipPolicyType::link(ptr, object);
        }
        return *this;
      }

      /// The object can be dereferenced like the held object
      objectType* operator->() const              { return object; }
      /// The object can be dereferenced like the held object
      objectType& operator*() const               { return *object; }

      bool operator== (const objectType* p) const { return object == p; }
      bool operator!= (const objectType* p) const { return object != p; }
      bool operator< (const objectType* p) const  { return object < p; }
      bool operator! () const { return object == 0; }
      operator bool () const  { return object != 0; }

      objectType* getPointer()              { return object; }
      const objectType* getPointer() const  { return object; }
      operator objectType* ()               { return object; }
      operator const objectType* () const   { return object; }
  };

}

#endif // TNTDB_SMARTPTR_H

