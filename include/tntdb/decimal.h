/*
 * Copyright (C) 2007,2012 Tommi Maekitalo, Mark Wright
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

#ifndef TNTDB_DECIMAL_H
#define TNTDB_DECIMAL_H

#include <string>
#include <limits>
#include <iosfwd>

namespace tntdb
{
class Decimal
{
public:
    typedef long long LongType;
    typedef unsigned long long UnsignedLongType;

private:

    // inf: _mantissa is empty, _exponent = numeric_limits<short>::max()
    // nan: _mantissa is empty, _exponent==0
    std::string _mantissa;  // just '0'-'9'
    short _exponent;
    bool _negative;

    Decimal(const std::string& mantissa, short exponent, bool negative)
      : _mantissa(mantissa),
        _exponent(exponent),
        _negative(negative)
      { }

    friend class Parser;

    LongType _getInteger(LongType min, LongType max, short exponent) const;
    UnsignedLongType _getUnsigned(UnsignedLongType max, short exponent) const;

    template <typename IntType> IntType _getInteger(short exponent) const
    {
        return _getInteger(std::numeric_limits<IntType>::min(),
                           std::numeric_limits<IntType>::max(),
                           exponent);
    }

    template <typename UnsignedType> UnsignedType _getUnsigned(short exponent) const
      { return _getUnsigned(std::numeric_limits<UnsignedLongType>::max(), exponent); }

    void _getInteger(short& ret, short exponent) const              { ret = _getInteger<short>(exponent); }
    void _getInteger(int& ret, short exponent) const                { ret = _getInteger<int>(exponent); }
    void _getInteger(long& ret, short exponent) const               { ret = _getInteger<long>(exponent); }
#ifdef HAVE_LONG_LONG
    void _getInteger(long long& ret, short exponent) const          { ret = _getInteger<long long>(exponent); }
#endif
    void _getInteger(unsigned short& ret, short exponent) const     { ret = _getUnsigned<unsigned short>(exponent); }
    void _getInteger(unsigned int& ret, short exponent) const       { ret = _getUnsigned<unsigned int>(exponent); }
    void _getInteger(unsigned long& ret, short exponent) const      { ret = _getUnsigned<unsigned long>(exponent); }
#ifdef HAVE_UNSIGNED_LONG_LONG
    void _getInteger(unsigned long long& ret, short exponent) const { ret = _getUnsigned<unsigned long long>(exponent); }
#endif

    void _setInteger(LongType l, short exponent);
    void _setUnsigned(UnsignedLongType l, short exponent);

public:
    class Parser;

    Decimal();

    explicit Decimal(long double value)
      { setDouble(value); }

    explicit Decimal(const std::string& value);

    Decimal(long mantissa, short exponent)
      { setInteger(mantissa, exponent); }

    static Decimal infinity()
      { return Decimal(std::string(), std::numeric_limits<short>::max(), false); }

    static Decimal nan()
      { return Decimal(std::string(), 0, false); }

    const std::string& mantissa() const
      { return _mantissa; }

    short exponent() const
      { return _exponent; }

    bool negative() const
      { return _negative; }

    bool isInfinity(bool positiveInfinity = true) const
      { return _negative != positiveInfinity && _mantissa.empty() && _exponent == std::numeric_limits<short>::max(); }

    bool isPositiveInfinity() const
      { return isInfinity(true); }

    bool isNegativeInfinity() const
      { return isInfinity(false); }

    bool isNaN() const
      { return _mantissa.empty() && _exponent == 0; }

    bool isZero() const
      { return _mantissa == "0"; }

    void setDouble(long double value);

    long double getDouble() const;

    void setInteger(short l, short exponent = 0)              { _setInteger(l, exponent); }
    void setInteger(int l, short exponent = 0)                { _setInteger(l, exponent); }
    void setInteger(long l, short exponent = 0)               { _setInteger(l, exponent); }
    void setInteger(long long l, short exponent = 0)          { _setInteger(l, exponent); }
    void setInteger(unsigned short l, short exponent = 0)     { _setUnsigned(l, exponent); }
    void setInteger(unsigned int l, short exponent = 0)       { _setUnsigned(l, exponent); }
    void setInteger(unsigned long l, short exponent = 0)      { _setUnsigned(l, exponent); }
    void setInteger(unsigned long long l, short exponent = 0) { _setUnsigned(l, exponent); }

    template <typename IntType>
    IntType getInteger(short exponent = 0) const
    {
        IntType ret;
        _getInteger(ret, exponent);
        return ret;
    }

    std::string toString() const;
    std::string toStringSci() const;
    std::string toStringFix() const;

    Decimal operator- () const
      { return Decimal(_mantissa, _exponent, !_negative); }

    bool operator== (const Decimal& other) const
      { return !isNaN() && _mantissa == other._mantissa && _exponent == other._exponent && _negative == other._negative; }

    bool operator!= (const Decimal& other) const
      { return !(*this == other); }

    bool operator< (const Decimal& other) const;

    bool operator> (const Decimal& other) const
      { return other < *this; }

    bool operator<= (const Decimal& other) const
      { return !(other < *this); }

    bool operator>= (const Decimal& other) const
      { return !(other > *this); }
};

std::istream& operator>> (std::istream& in, Decimal& dec);
std::ostream& operator<< (std::ostream& out, const Decimal& dec);
}

#endif // TNTDB_DECIMAL_H

