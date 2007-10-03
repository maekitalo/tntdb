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

  public:
    /// Initializes the Decimal-object with empty values.
    Decimal();
    /// Initializes the Decimal-object with the given double value.
    explicit Decimal(double value);

    Decimal(int64_t man, ExponentType exp);
    
    /// Initializes the Decimal-object with the given MantissaType mantissa
    /// and ExponentType exponent
    Decimal(MantissaType man, ExponentType exp, FlagsType f, PrintFlagsType pf = infinityShort);
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
    IntegerType getInteger(RoundingAlgorithmType roundingAlgorithm = round) const throw(std::overflow_error);

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
      
    /// Print this Decimal number.  If out.precision() != 0, then this
    /// decimal number is printed with out.precision() significant digits.
    std::ostream &print(std::ostream &out) const;

    /// Print this Decimal number, with optional printFlags which only affect
    /// how positive and negative infinity are printed.
    /// If out.precision() != 0, then this decimal number is printed with
    /// out.precision() significant digits.
    std::ostream &print(std::ostream &out, PrintFlagsType printFlags) const;
    
    /// Read a Decimal number.
    /// @param in input stream
    /// @param ignoreOverflowReadingFraction if true, ignore overflow errors
    /// while reading the fractional part, and set the mantissa to the the
    /// value before the overflow.  This is useful while converting from
    /// float or double values to Decimal.  The full range of significant
    /// digits that the Decimal can hold can be read in the conversion like:
    /// std::ostringstream ostr;
    /// ostr.precision(24);
    /// ostr << num;
    /// read(ostr.str(), true);
    std::istream &read(std::istream &in, bool ignoreOverflowReadingFraction = false);

    void init(MantissaType m, ExponentType e, FlagsType f = positive, PrintFlagsType pf = infinityShort);

  protected:
    static void printFraction(std::ostream &out,
                              ExponentType fracDigits,
                              MantissaType fractional);

    /// Multiply an unsigned integer type by 10, checking for overflow.
    /// @param n on input: the number to multply by 10, on output, the
    /// result of the multiplication by 10.
    /// @return true if overflow detected, else false;
    template <typename T>
    static bool overflowDetectedInUnsignedMultiplyByTen(T &n);
    
    /// Divide an unsigned integer type by a power of 10 divisor, checking
    /// for overflow.
    /// @param dividend the unsigned integer dividend.
    /// @param quotient the result of the division by the power of 10 divisor.
    /// @param remainder the remainder result of the division by the power of 10 divisor.
    /// @param divisorPowerOfTenDigits divide by 10^divisorPowerOfTenDigits.
    /// @return true if overflow detected, else false;
    template <typename ManType>
    static void unsignedDivideByPowerOfTen(const ManType dividend,
                                           ManType &quotient,
                                           ManType &remainder,
                                           ManType divisorPowerOfTenDigits) throw(std::overflow_error);
  };
  
  template <typename T>
  bool Decimal::overflowDetectedInUnsignedMultiplyByTen(T &n)
  {
    bool overflowDetected = false;
    T nTimes2 = n << 1;
    T nTimes4 = n << 2;
    T nTimes8 = n << 3;
    T nTimes10 = (nTimes2 + nTimes8);
    if ((nTimes2 < n) || (nTimes4 < nTimes2) || (nTimes8 < nTimes4) || (nTimes10 < nTimes8))
      overflowDetected = true;
    else
    {
      n = nTimes10;
    }
    return overflowDetected;
  }

  template <typename IntegerType> 
  IntegerType Decimal::numberOfDigits(IntegerType n) const
  {
    IntegerType multiplier = IntegerType(Base);
    IntegerType noDigits = 1;
    IntegerType abs = n;
    if (n < 0)
      abs = -n;
    bool overflowDetected = false;
    for (; !overflowDetected && (abs > multiplier); ++noDigits)
    {
      overflowDetected = Decimal::overflowDetectedInUnsignedMultiplyByTen(multiplier);
    }
    return noDigits;
  }
  
  template <typename ManType>
  void Decimal::unsignedDivideByPowerOfTen(const ManType dividend,
                                           ManType &quotient,
                                           ManType &remainder,
                                           ManType divisorPowerOfTenDigits) throw(std::overflow_error)
  {
    ManType divisorExponentPowerOfTenDigitsRemaining = divisorPowerOfTenDigits;
    // If the divisorPowerOfTenDigits is larger than
    // maxDivisorDigits, then do 2 (or more if required) divides
    // to avoid unsigned integer overflow in the multiplication calculating
    // the divisor.
    const ManType maxDivisorDigits = 19;
    ManType exponentDivisor = ManType(Base);
    ManType previousExponentDivisor = ManType(1);
    bool overflowDetected = false;
    if (divisorExponentPowerOfTenDigitsRemaining > maxDivisorDigits)
    {
      // The first divide is to make divisorExponentPowerOfTenDigitsRemaining an even multiple of
      // maxDivisorDigits.
      ManType divideChunksRemaining = divisorExponentPowerOfTenDigitsRemaining / maxDivisorDigits;
      ManType firstDivideDigits = divisorExponentPowerOfTenDigitsRemaining % maxDivisorDigits;
      for (int i = 1; (i < firstDivideDigits) && !overflowDetected; ++i)
      {
        previousExponentDivisor = exponentDivisor;
        overflowDetected = overflowDetectedInUnsignedMultiplyByTen(exponentDivisor);
      }
      if (overflowDetected)
        throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::unsignedDivideByPowerOfTen()"));
      quotient = ManType(dividend) / exponentDivisor;
      divisorExponentPowerOfTenDigitsRemaining -= firstDivideDigits;
      --divideChunksRemaining;
      exponentDivisor = ManType(Base);
      // If divisorExponentPowerOfTenDigitsRemaining is still larger than maxDivisorDigits,
      // then do more divides until it is equal to maxDivisorDigits.
      if (divideChunksRemaining > 1)
      {
        // Calculate the 10^maxDivisorDigits divisor
        for (int i = 1; (i < maxDivisorDigits) && !overflowDetected; ++i)
        {
          previousExponentDivisor = exponentDivisor;
          overflowDetected = overflowDetectedInUnsignedMultiplyByTen(exponentDivisor);
        }
        if (overflowDetected)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::unsignedDivideByPowerOfTen()"));
        // Do all but the last divide
        while (divideChunksRemaining > 1)
        {
          quotient = ManType(dividend) / exponentDivisor;
          --divideChunksRemaining;
        }
        exponentDivisor = ManType(Base);
      }
    }
    for (int i = 1; (i < divisorExponentPowerOfTenDigitsRemaining) && !overflowDetected; ++i)
    {
      previousExponentDivisor = exponentDivisor;
      overflowDetected = overflowDetectedInUnsignedMultiplyByTen(exponentDivisor);
    }
    if (overflowDetected)
      throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::unsignedDivideByPowerOfTen()"));
    quotient = ManType(dividend) / exponentDivisor;
    remainder = ManType(dividend) % exponentDivisor;
  }
  
  template <typename ManType>
  void Decimal::getIntegralFractionalExponent(ManType &integral,
                                              ManType &fractional,
                                              ExponentType &ex,
                                              ExponentType optionalUserSpecifiedExponentOffset) const throw(std::overflow_error)
  {
    ManType integralPart = mantissa;
    ManType fractionalPart = ManType(0);
    ExponentType exp = exponent - optionalUserSpecifiedExponentOffset;
    if ((optionalUserSpecifiedExponentOffset != 0) && (integralPart != 0))
    {
      if (optionalUserSpecifiedExponentOffset >= 0)
      {
        ManType previousIntegralPart = ManType(0);
        bool overflowDetected = false;
        for (int i = 0; (i < optionalUserSpecifiedExponentOffset) && !overflowDetected; ++i)
        {
          previousIntegralPart = integralPart;
          overflowDetected = overflowDetectedInUnsignedMultiplyByTen(integralPart);
        }
        if (overflowDetected)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::getIntegralFractionalExponent()"));
      }
      else
      {
        ManType absOptionalUserSpecifiedExponentOffset = ManType(-optionalUserSpecifiedExponentOffset);
        Decimal::unsignedDivideByPowerOfTen(ManType(mantissa), integralPart, fractionalPart, absOptionalUserSpecifiedExponentOffset);
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
  IntegerType Decimal::getInteger(RoundingAlgorithmType roundingAlgorithm) const throw(std::overflow_error)
  {
    ExponentType exp = 0;
    IntegerType quotient = 0;
    IntegerType remainder = 0;
    getIntegralFractionalExponent<IntegerType>(quotient,
                                               remainder,
                                               exp,
                                               exponent);
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

  template <>
  inline bool Decimal::getInteger(RoundingAlgorithmType roundingAlgorithm) const throw(std::overflow_error)
  {
    if ((flags & (infinity | NaN)) || (mantissa != 0))
      return true;
    else
      return false;
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
    // 2^64 = 18446744073709551616, which has 20 digits.
    // Hence a precision of 20 digits could overflow.
    // If it does, hopefully it will be while reading
    // the fractional part of the floating point number,
    // in which case read() can use the mantissa value before
    // the overflow.
    std::ostringstream ostr;
    ostr.precision(24);
    ostr << num;
    std::string numStr(ostr.str());
    std::istringstream istr(numStr);
    read(istr, true);
  }
  
  /// Print this Decimal number.  If out.precision() != 0, then this
  /// decimal number is printed with out.precision() significant digits.
  std::ostream &operator<<(std::ostream &out, const Decimal& decimal);

  /// Read a Decimal number.
  std::istream &operator>>(std::istream &in, Decimal& decimal);
}

#endif // TNTDB_DECIMAL_H
