/* 
 * Copyright (C) 2007 Tommi Maekitalo, Mark Wright
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

#ifndef TNTDB_DECIMAL_H
#define TNTDB_DECIMAL_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace tntdb
{
  /**
   * This class holds a decimal number.
   */
  class Decimal
  {
  public:
    typedef uint64_t MantissaType;
    typedef int32_t ExponentType;
    typedef int8_t FlagsType;
    typedef int8_t PrintFlagsType;
    typedef int16_t FractionDigitsType;
    enum { Base = 10 };
    enum
    {
      positive = 0x01,
      infinity = 0x02,
      negativeInfinity = infinity,
      positiveInfinity = infinity | positive,
      NaN = 0x04
    };
    enum
    {
      infinityShort,   // Affects output only, print Inf or -Inf.
      infinityLong,    // Affects output only, print Infinity or -Infinity instead of Inf or -Inf.
      infinityTilde    // Affects output only, print ~ or -~ (as used in Oracle) instead of Inf or -Inf.
    };  
    enum RoundingAlgorithmType
    {
      truncate,
      round,
      bankersRound
    };
  private:
    MantissaType mantissa;
    ExponentType exponent;
    FlagsType flags;
    PrintFlagsType defaultPrintFlags;
    FractionDigitsType fractionDigits;

  public:
    /// Initializes the Decimal-object with empty values.
    Decimal();
    /// Initializes the Decimal-object with the given double value.
    explicit Decimal(double value);

    Decimal(int64_t man, ExponentType exp);
    
    /// Initializes the Decimal-object with the given MantissaType mantissa
    /// and ExponentType exponent
    Decimal(MantissaType man, ExponentType exp, FlagsType f, FractionDigitsType fd = 0, PrintFlagsType pf = infinityShort);
    /// Return the decimal mantissa.
    MantissaType getMantissa() const;
    /// Return the base 10 exponent.
    ExponentType getExponent() const;

    /// Return true if this Decimal number is positive, else return false.
    bool isPositive() const
      { return flags & positive; }
    
    /// Return true if this Decimal number is either positive or negative
    /// infinity, else return false.
    bool isInfinity() const;

    /// If postitiveInfinity is true, then return true if this
    /// Decimal number is postitive infinity.
    /// Else if postitiveInfinity is false, then return true if this
    /// Decimal number is negative infinity.
    /// Else return false.
    bool isInfinity(bool positiveInfinity) const;
    
    /// Return true if this Decimal number is Not a Number.
    /// Else return false.
    bool isNaN() const;
    
    /// Return true if this Number object is zero, else return false;
    bool isZero() const;
      
    /// Split this decimal number into integral part, fractional
    /// and exponent parts.  An optional user specified exponent
    /// offset can be used to first scale the decimal number.
    template <typename ManType>
    void getIntegralFractionalExponent(ManType &integral,
                                       ManType &fractional,
                                       ExponentType &ex,
                                       ExponentType optionalUserSpecifiedExponentOffset = 0) const throw(std::overflow_error);

    /// Return the number of decimal digits in n.
    template <typename IntegerType> 
    IntegerType numberOfDigits(IntegerType n) const;
    
    /// Return this number as a C++ integer type.
    template <typename IntegerType>
    IntegerType getInteger(RoundingAlgorithmType roundingAlgorithm = round,
                           ExponentType optionalUserSpecifiedExponentOffset = 0) const throw(std::overflow_error);

    /// Return this number as a C++ floating point type.
    template <typename FloatingPointType>
    FloatingPointType getFloatingPoint() const;

    /// Return this number as a C++ int.
    int getInt() const throw(std::overflow_error)
      { return getInteger<int>(); }

    /// Return this number as a C++ int32_t.
    int32_t getInt32() const throw(std::overflow_error)
      { return getInteger<int32_t>(); }
      
    /// Return this number as a C++ unsigned.
    unsigned getUnsigned() const throw(std::overflow_error)
      { return getInteger<unsigned>(); }

    /// Return this number as a C++ uint32_t.
    uint32_t getUnsigned32() const throw(std::overflow_error)
      { return getInteger<uint32_t>(); }

    /// Return this number as a C++ int64_t.
    int64_t getInt64() const throw(std::overflow_error)
      { return getInteger<int64_t>(); }

    /// Return this number as a C++ uint64_t.
    uint64_t getUnsigned64() const throw(std::overflow_error)
      { return getInteger<uint64_t>(); }
    
    /// Convert to a C++ float.
    float getFloat() const
      { return getFloatingPoint<float>(); }

    /// Convert to a C++ double.
    double getDouble() const
      { return getFloatingPoint<double>(); }

    /// Set this this tntdb::Decimal object to the value of the given
    /// integer type.
    template <typename IntegerType>
    void setInteger(IntegerType num);

    /// Set this this tntdb::Decimal object to the value of the given
    /// floating point type.
    template <typename FloatingPointType>
    void setFloatingPoint(FloatingPointType num);
    
    /// Set this this tntdb::Decimal object to the value of the given
    /// integer type mantissa and base 10 exponent.
    template <typename IntegerType>
    void setDecimalInteger(IntegerType num, int32_t exponent);
      
    /// Set this this tntdb::Decimal object to the value of the given int.
    void setInt(int num)
      { setInteger<int>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int32_t.
    void setInt32(int32_t num)
      { setInteger<int32_t>(num); }
      
    /// Set this this tntdb::Decimal object to the value of the given unsigned.
    void setUnsigned(unsigned num)
      { setInteger<unsigned>(num); }

    /// Set this this tntdb::Decimal object to the value of the given uint32_t.
    void setUnsigned32(uint32_t num)
      { setInteger<uint32_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int64_t.
    void setInt64(int64_t num)
      { setInteger<int64_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given uint64_t.
    void setUnsigned64(uint64_t num)
      { setInteger<int64_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int64_t.
    void setDecimalInt64(int64_t num, int32_t exp)
      { setDecimalInteger<int64_t>(num, exp); }

    /// Set this this tntdb::Decimal object to the value of the given uint64_t.
    void setDecimalUnsigned64(uint64_t num, int32_t exp)
      { setDecimalInteger<int64_t>(num, exp); }

    /// Set this this tntdb::Decimal object to the value of the given float.
    void setFloat(float num)
      { setFloatingPoint<float>(num); }

    /// Set this this tntdb::Decimal object to the value of the given double.
    void setDouble(double num)
      { setFloatingPoint<double>(num); }
    
    // Return this Decimal number as a string.
    std::string toString() const;
      
    /// Print this Decimal number, ignoring std::ostream flags, fill and width.
    std::ostream &print(std::ostream &out) const;

    /// Print this Decimal number, ignoring std::ostream flags, fill and width.
    std::ostream &print(std::ostream &out, PrintFlagsType printFlags) const;
    
    /// Print this Decimal number, using some std::ostream flags, fill and width.
    std::ostream &printFormatted(std::ostream &out) const;

    /// Print this Decimal number, using some std::ostream flags, fill and width.
    std::ostream &printFormatted(std::ostream &out, PrintFlagsType printFlags) const;
    
    /// Read a Decimal number.
    std::istream &read(std::istream &in);

    void init(MantissaType m, ExponentType e, FlagsType f = positive, FractionDigitsType fd = 0, PrintFlagsType pf = infinityShort);
  };

  template <typename IntegerType> 
  IntegerType Decimal::numberOfDigits(IntegerType n) const
  {
    IntegerType multiplier = IntegerType(10);
    IntegerType previousMultiplier = IntegerType(1);
    IntegerType noDigits = 1;
    IntegerType abs = n;
    if (n < 0)
      abs = -n;
    for (; (abs < multiplier) && (multiplier > previousMultiplier); ++noDigits)
    {
      previousMultiplier = multiplier;
      multiplier *= IntegerType(Base);
    }
    return noDigits;
  }
  
  template <typename ManType>
  void Decimal::getIntegralFractionalExponent(ManType &integral,
                                              ManType &fractional,
                                              ExponentType &ex,
                                              ExponentType optionalUserSpecifiedExponentOffset) const throw(std::overflow_error)
  {
    ManType integralPart = mantissa;
    ManType fractionalPart = ManType(0);
    ExponentType exp = exponent + optionalUserSpecifiedExponentOffset;
    if ((exp != 0) && (integralPart != 0))
    {
      if (exp >= 0)
      {
        ManType previousIntegralPart = ManType(0);
        for (int i = 0; (i < exp) && (integralPart > previousIntegralPart); ++i)
        {
          previousIntegralPart = integralPart;
          integralPart *= ManType(Base);
        }
        if (integralPart <= previousIntegralPart)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::getIntegralFractionalExponent()"));
      }
      else
      {
        ExponentType absExponent = -exp;
        ManType exponentDivisor = ManType(Base);
        ManType previousExponentDivisor = ManType(1);
        for (int i = 1; (i < absExponent) && (exponentDivisor > previousExponentDivisor); ++i)
        {
          previousExponentDivisor = exponentDivisor;
          exponentDivisor *= ManType(Base);
        }
        if (exponentDivisor <= previousExponentDivisor)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::getIntegralFractionalExponent()"));
        integralPart = ManType(mantissa) / exponentDivisor;
        fractionalPart = ManType(mantissa) % exponentDivisor;
      }
    }
    // if positive, or ManType is an unsigned integer type
    if ((flags & positive) || (ManType(-1) > ManType(0)))
      integral = integralPart;
    else
      integral = -integralPart;
    fractional = fractionalPart;
    ex = exp;
  }
  
  template <typename IntegerType>
  IntegerType Decimal::getInteger(RoundingAlgorithmType roundingAlgorithm,
                                  ExponentType optionalUserSpecifiedExponentOffset) const throw(std::overflow_error)
  {
    ExponentType exp = 0;
    IntegerType quotient = 0;
    IntegerType remainder = 0;
    getIntegralFractionalExponent<IntegerType>(quotient,
                                               remainder,
                                               exp,
                                               optionalUserSpecifiedExponentOffset);
    if (exp >= 0)
      return quotient;

    ExponentType absExponent = -exp;
    IntegerType oneHalf = IntegerType(Base / 2);
    for (int i = 1; i < absExponent; ++i)
    {
      oneHalf *= IntegerType(Base);
    }

    switch(roundingAlgorithm)
    {
      case round:
        // if positive, or IntegerType is an unsigned integer type
        if ((flags & positive) || (IntegerType(-1) > IntegerType(0)))
        {
          return (remainder >= oneHalf) ? quotient + IntegerType(1) : quotient;
        }
        else
        {
          return (remainder >= oneHalf) ? -(quotient + IntegerType(1)) : -quotient;
        }

      case bankersRound:
        if ((flags & positive) || (IntegerType(-1) > IntegerType(0)))
        {
          return (remainder > oneHalf) ? quotient + IntegerType(1) : quotient;
        }
        else
        {
          return (remainder > oneHalf) ? -(quotient + IntegerType(1)) : -quotient;
        }
    }

    // default is roundingAlgorithm = truncate
    return ((flags & positive) || (IntegerType(-1) > IntegerType(0))) ? quotient : -quotient;
  }
  
  template <typename FloatingPointType>
  FloatingPointType Decimal::getFloatingPoint() const
  {
    if (exponent >= 0)
    {
      FloatingPointType exponentMultiplier = FloatingPointType(1);
      for (int i = 0; i < exponent; ++i)
        exponentMultiplier *= FloatingPointType(Base);
      FloatingPointType x = FloatingPointType(mantissa) * exponentMultiplier;
      if (flags & positive)
        return x;
      else
        return -x;
    }
    else
    {
      ExponentType absExponent = -exponent;
      FloatingPointType exponentDivisor = FloatingPointType(Base);
      for (int i = 1; i < absExponent; ++i)
      {
        exponentDivisor *= FloatingPointType(Base);
      }
      FloatingPointType quotient = FloatingPointType(mantissa) / exponentDivisor;
      if (flags & positive)
      {
        return quotient;
      }
      else
      {
        return -quotient;
      }
    }
  }
    
  template <typename IntegerType>
  void Decimal::setInteger(IntegerType num)
  {
    if (num >= 0)
    {
      flags |= positive;
      mantissa = MantissaType(num);
    }
    else
    {
      flags &= ~positive;
      mantissa = MantissaType(-num);
    }
    flags &= ~(infinity | NaN);
    exponent = 0;
  }

  template <typename FloatingPointType>
  void Decimal::setFloatingPoint(FloatingPointType num)
  {
    std::ostringstream ostr;
    ostr << num;
    std::istringstream istr(ostr.str());
    read(istr);
  }
  
  std::ostream &operator<<(std::ostream &out, const Decimal& decimal);
  std::istream &operator>>(std::istream &in, Decimal& decimal);
}

#endif // TNTDB_DECIMAL_H
