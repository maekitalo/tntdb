/*
 * Copyright (C) 2005,2010 Tommi Maekitalo
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

#ifndef TNTDB_BITS_VALUE_H
#define TNTDB_BITS_VALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <tntdb/blob.h>
#include <memory>
#if __cplusplus >= 201703L
#include <optional>
#endif


namespace tntdb
{
/**
 * The class Value represents a value, which is fetched from the database.
 */
class Value
{
    std::shared_ptr<IValue> _value;

public:
    Value() { }

    explicit Value(const std::shared_ptr<IValue>& value)
      : _value(value)
      { }

    //@{
    /**
     * Explicit data-access.
     * The getXXX-methods return the value in the requested type.
     * If the value can't be converted to the requested type, a exception of
     * type tntdb::TypeError is thrown.
     */
    /// return true, when value is null
    bool isNull() const                 { return !_value || _value->isNull(); }
    /// return true, when value represents boolean true.
    bool getBool() const                { return _value->getBool(); }
    /// tries to convert value into an short.
    short getShort() const              { return _value->getShort(); }
    /// tries to convert value into an int.
    int getInt() const                  { return _value->getInt(); }
    /// tries to convert value into an long.
    long getLong() const                { return _value->getLong(); }
    /// tries to convert value into an unsigned short.
    unsigned short getUnsignedShort() const { return _value->getUnsignedShort(); }
    /// tries to convert value into an unsigned.
    unsigned getUnsigned() const        { return _value->getUnsigned(); }
    /// tries to convert value into an unsigned long.
    unsigned long getUnsignedLong() const { return _value->getUnsignedLong(); }
    /// tries to convert value into an int32_t.
    int32_t getInt32() const            { return _value->getInt32(); }
    /// tries to convert value into an uint32_t.
    uint32_t getUnsigned32() const      { return _value->getUnsigned32(); }
    /// tries to convert value into an int64_t.
    int64_t getInt64() const            { return _value->getInt64(); }
    /// tries to convert value into an uint64_t.
    uint64_t getUnsigned64() const      { return _value->getUnsigned64(); }
    /// tries to convert value into a Decimal.
    Decimal getDecimal() const          { return _value->getDecimal(); }
    /// tries to convert value into an float.
    float getFloat() const              { return _value->getFloat(); }
    /// tries to convert value into an double.
    double getDouble() const            { return _value->getDouble(); }
    /// returns the first character of the text-representation.
    char getChar() const                { return _value->getChar(); }
    /// returns the value as a string.
    std::string getString() const
      { std::string ret; _value->getString(ret); return ret; }
    /// fills the passed string with the value.
    /// this might be slightly more efficient than just returning a new
    /// string since one copy is saved.
    void getString(std::string& ret) const
      { _value->getString(ret); }
    /// returns the value as a unicode string.
    cxxtools::String getUString() const
      { cxxtools::String ret; _value->getUString(ret); return ret; }
    void getUString(cxxtools::String& ret) const
      { _value->getUString(ret); }
    /// Returns the value as a blob.
    /// This is more or less an alias to getString just to stress, that
    /// the data is truly binary and not some text value.
    Blob getBlob() const
      { Blob ret; _value->getBlob(ret); return ret; }
    /// Returns the value as a blob.
    void getBlob(Blob& blob) const
      { _value->getBlob(blob); }
    /// returns the value as a Date.
    Date getDate() const                { return _value->getDate(); }
    /// returns the value as a Time.
    Time getTime() const                { return _value->getTime(); }
    /// returns the value as a Datetime.
    Datetime getDatetime() const        { return _value->getDatetime(); }
    //@}

    /**
     * Explicit data-access.
     * The get-template return false if the value is null. Otherwise the
     * passed reference is filled with the value.
     * If the value can't be converted to the requested type, a exception of
     * type tntdb::TypeError is thrown.
     *
     * In contrast to the getXXX-methods the type is not specified explictely
     * but determined by the passed reference.
     *
     * The extraction is actually done using the operator>> with a l-value of
     * const Value& and a r-value of a reference to the actual type. This
     * operator is defined for standard types and may be defined for user
     * defined types.
     */
    template <typename T>
    bool getValue(T& ret) const
      { return *this >> ret; }

#if __cplusplus >= 201703L
    template <typename T>
    bool getValue(T&& ret) const
      { return *this >> std::move(ret); }
#endif

    /// Shorter name for getValue.
    template <typename T>
    bool get(T& ret) const
      { return *this >> ret; }

#if __cplusplus >= 201703L
    template <typename T>
    bool get(T&& ret) const
      { return *this >> std::move(ret); }
#endif

    /// Returns true, if this class is not connected to a actual statement.
    bool operator!() const              { return !_value; }
    ///  Returns the actual implementation-class.
    std::shared_ptr<IValue> getImpl() const       { return _value; }
};

//@{
/**
  Extraction operators for standard types.
 */
inline bool operator>> (const Value& value, bool& out)
{
    if (value.isNull())
        return false;

    out = value.getBool();
    return true;
}

inline bool operator>> (const Value& value, short& out)
{
    if (value.isNull())
        return false;

    out = value.getShort();
    return true;
}

inline bool operator>> (const Value& value, int& out)
{
    if (value.isNull())
          return false;

    out = value.getInt();
    return true;
}

inline bool operator>> (const Value& value, long& out)
{
    if (value.isNull())
        return false;

    out = value.getLong();
    return true;
}

inline bool operator>> (const Value& value, unsigned short& out)
{
    if (value.isNull())
        return false;

    out = value.getUnsignedShort();
    return true;
}

inline bool operator>> (const Value& value, unsigned& out)
{
    if (value.isNull())
        return false;

    out = value.getUnsigned();
    return true;
}

inline bool operator>> (const Value& value, unsigned long& out)
{
    if (value.isNull())
        return false;

    out = value.getUnsignedLong();
    return true;
}

inline bool operator>> (const Value& value, Decimal& out)
{
    if (value.isNull())
        return false;

    out = value.getDecimal();
    return true;
}

inline bool operator>> (const Value& value, float& out)
{
    if (value.isNull())
        return false;

    out = value.getFloat();
    return true;
}

inline bool operator>> (const Value& value, double& out)
{
    if (value.isNull())
        return false;

    out = value.getDouble();
    return true;
}

inline bool operator>> (const Value& value, char& out)
{
    if (value.isNull())
        return false;

    out = value.getChar();
    return true;
}

inline bool operator>> (const Value& value, std::string& out)
{
    if (value.isNull())
        return false;

    value.getString(out);
    return true;
}

inline bool operator>> (const Value& value, cxxtools::String& out)
{
    if (value.isNull())
        return false;

    value.getUString(out);
    return true;
}

inline bool operator>> (const Value& value, Blob& out)
{
    if (value.isNull())
        return false;

    value.getBlob(out);
    return true;
}

inline bool operator>> (const Value& value, Date& out)
{
    if (value.isNull())
        return false;

    out = value.getDate();
    return true;
}

inline bool operator>> (const Value& value, Time& out)
{
    if (value.isNull())
        return false;

    out = value.getTime();
    return true;
}

inline bool operator>> (const Value& value, Datetime& out)
{
    if (value.isNull())
        return false;

    out = value.getDatetime();
    return true;
}

#if __cplusplus >= 201703L
template <typename T>
bool operator>> (const Value& value, std::optional<T>& out)
{
    if (value.isNull())
    {
        out.reset();
        return false;
    }

    out = T();
    value >> *out;
    return true;
}
#endif

//@}

}

#endif // TNTDB_BITS_VALUE_H

