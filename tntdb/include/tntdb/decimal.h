/*
 * Copyright (C) 2007 Tommi Maekitalo, Mark Wright
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
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>

namespace tntdb
{
  /**
   * This class holds a decimal floating point number.  It is necessary to
   * convert it to some other type to perform arithmetic and other operations.
   * Decimal is implemented with a 64 bit unsigned integer mantissa, and a
   * 32 bit exponent.  For conversions from integer types, strings, and reading
   * from the database, Decimal tries to maintain accuracy if the result will
   * fit in the 64 bit unsigned mantissa and exponent.  When converting to
   * integer types, Decimal will throw std::overflow exception if the result
   * will not fit.  For converting to binary floating point numbers, binary
   * floating point is used in the conversion, and the result is an
   * approximation.
   */
  class Decimal
  {
  public:
    typedef uint64_t MantissaType;
    typedef int32_t ExponentType;
    typedef int8_t FlagsType;
    typedef int8_t PrintFlagsType;
    enum { Base = 10 };
    /// Flags used for denoting positive or negative, infinity and not a number.
    enum FlagsTypeEnum
    {
      positive = 0x01,                          ///< Set if this Decimal is positive, else this Decimal is negative.
      infinity = 0x02,                          ///< Set if this Decimal is positive or negative infinity.
      negativeInfinity = infinity,              ///< Negative infinity.
      positiveInfinity = infinity | positive,   ///< Positive infinity.
      NaN = 0x04                                ///< Not a Number.
    };
    /// How infinity is printed on output.
    enum InfinityOutputType
    {
      infinityShort,   ///< Affects output only, print Inf or -Inf.
      infinityLong,    ///< Affects output only, print Infinity or -Infinity instead of Inf or -Inf.
      infinityTilde    ///< Affects output only, print ~ or -~ (as used in Oracle) instead of Inf or -Inf.
    };
    /// Rounding algorithm.
    enum RoundingAlgorithmType
    {
      truncate,        ///< 1.1, 1.5, and 1.6 and all rounded down to 1.0.
      round,           ///< 1.1 is rounded down to 1.0, 1.5, and 1.6 are rounded up to 2.0.
      bankersRound     ///< 1.1 and 1.5 are rounded down to 1.0, 1.6 is rounded up to 2.0.
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
    /// @param value double to initialize it with.
    explicit Decimal(double value);

    /// Initialize this Decimal-object with the given decimal mantissa and exponent.
    /// @param man integer decimal mantissa value to set this Decimal number to.
    /// @param exp integer base 10 exponent to set this Decimal number to.
    Decimal(int64_t man, ExponentType exp);

    /// Initializes the Decimal-object with the given MantissaType mantissa
    /// and ExponentType exponent
    /// @param man integer decimal mantissa value to set this Decimal number to.
    /// @param exp integer base 10 exponent to set this Decimal number to.
    /// @param f the flags, need to specify positive or negative. @link Decimal::FlagsTypeEnum @endlink
    /// @param pf the print flags for infinity and not a number. @link Decimal::InfinityOutputType @endlink
    Decimal(MantissaType man, ExponentType exp, FlagsType f, PrintFlagsType pf = infinityShort);
    /// Return the decimal mantissa.
    /// @return the decimal mantissa.
    MantissaType getMantissa() const;
    /// Return the base 10 exponent.
    /// @return the base 10 exponent.
    ExponentType getExponent() const;

    /// Is this Decimal number positive?
    /// @return true if this Decimal number is positive, else return false.
    bool isPositive() const
      { return flags & positive; }

    /// Is this Decimal number positive or negative infinity?
    /// @return true if this Decimal number is either positive or negative
    /// infinity, else return false.
    bool isInfinity() const;

    /// Is this Decimal number positive or negative infinity?
    /// @param positiveInfinity
    /// @return If postitiveInfinity is true, then return true if this
    /// Decimal number is postitive infinity.
    /// Else if postitiveInfinity is false, then return true if this
    /// Decimal number is negative infinity.
    /// Else return false.
    bool isInfinity(bool positiveInfinity) const;

    /// Is this Decimal not a number?
    /// @return true if this Decimal number is Not a Number.
    /// Else return false.
    bool isNaN() const;

    /// Is this Decimal number zero?
    /// @return true if this Number object is zero, else return false.
    bool isZero() const;

    /// Split this decimal number into integral part, fractional
    /// and exponent parts.  An optional user specified exponent
    /// offset can be used to first scale the decimal number.
    /// @param integral the part of the decimal floating point
    /// number to the left of the decimal point.
    /// @param fractional the part to the decimal floating point
    /// number to the right of the decimal point.
    /// @param ex the exponent of the decimal floating point number
    /// @param optionalUserSpecifiedExponentOffset optional user
    /// specified exponent offset can be used to first scale the decimal number.
    /// @throw std::overflow_error if the result will not fit
    template <typename ManType>
    void getIntegralFractionalExponent(ManType &integral,
                                       ManType &fractional,
                                       ExponentType &ex,
                                       ExponentType optionalUserSpecifiedExponentOffset = 0) const throw(std::overflow_error);

    /// Return the number of decimal digits in n.
    template <typename IntegerType>
    IntegerType numberOfDigits(IntegerType n) const;

    /// Return this number as a C++ integer type.
    /// @param roundingAlgorithm @link Decimal::RoundingAlgorithmType @endlink
    /// @return integer result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    template <typename IntegerType>
    IntegerType getInteger(RoundingAlgorithmType roundingAlgorithm = round) const throw(std::overflow_error);

    /// Return this number as a C++ floating point type.
    /// @return binary floating point result, which is computed
    /// with binary floating point arithmetic, and hence is an approximation.
    template <typename FloatingPointType>
    FloatingPointType getFloatingPoint() const;

    /// Return this decimal number rounded as a C++ short.
    /// @return int result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    short getShort() const throw(std::overflow_error)
      { return getInteger<short>(); }

    /// Return this decimal number rounded as a C++ int.
    /// @return int result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    int getInt() const throw(std::overflow_error)
      { return getInteger<int>(); }

    /// Return this decimal number rounded as a C++ long.
    /// @return long result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    long getLong() const throw(std::overflow_error)
      { return getInteger<long>(); }

    /// Return this decimal number rounded as a C++ int32_t.
    /// @return int32_t result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    int32_t getInt32() const throw(std::overflow_error)
      { return getInteger<int32_t>(); }

    /// Return this decimal number rounded as a C++ unsigned.
    /// @return unsigned result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    unsigned short getUnsignedShort() const throw(std::overflow_error)
      { return getInteger<unsigned short>(); }

    /// Return this decimal number rounded as a C++ unsigned.
    /// @return unsigned result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    unsigned getUnsigned() const throw(std::overflow_error)
      { return getInteger<unsigned>(); }

    /// Return this decimal number rounded as a C++ unsigned long.
    /// @return unsigned long result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    unsigned long getUnsignedLong() const throw(std::overflow_error)
      { return getInteger<unsigned long>(); }

    /// Return this decimal number rounded as a C++ uint32_t.
    /// @return uint32_t result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    uint32_t getUnsigned32() const throw(std::overflow_error)
      { return getInteger<uint32_t>(); }

    /// Return this decimal number rounded as a C++ int64_t.
    /// @return int64_t result if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    int64_t getInt64() const throw(std::overflow_error)
      { return getInteger<int64_t>(); }

    /// Return this decimal number rounded as a C++ uint64_t.
    /// @return uint64_t number, if the result will fit.
    /// @throw std::overflow_error if the result will not fit
    uint64_t getUnsigned64() const throw(std::overflow_error)
      { return getInteger<uint64_t>(); }

    /// Convert to a C++ float.
    /// @return float result, which is computed with binary floating point
    /// arithmetic, and hence is an approximation.
    float getFloat() const
      { return getFloatingPoint<float>(); }

    /// Convert to a C++ double.
    /// @return double result, which is computed with binary floating point
    /// arithmetic, and hence is an approximation.
    double getDouble() const
      { return getFloatingPoint<double>(); }

    /// Set this this tntdb::Decimal object to the value of the given
    /// integer type.
    /// @param num integer value to set this Decimal number to.
    template <typename IntegerType>
    void setInteger(IntegerType num);

    /// Set this this tntdb::Decimal object to the value of the given
    /// floating point type.
    /// @param num floating point value to set this Decimal number to.
    template <typename FloatingPointType>
    void setFloatingPoint(FloatingPointType num);

    /// Set this this tntdb::Decimal object to the value of the given
    /// integer type mantissa and base 10 exponent.
    /// @param num integer decimal mantissa value to set this Decimal number to.
    /// @param exponent integer base 10 exponent to set this Decimal number to.
    template <typename IntegerType>
    void setDecimalInteger(IntegerType num, int32_t exponent);

    /// Set this this tntdb::Decimal object to the value of the given int.
    /// @param num integer value to set this Decimal number to.
    void setInt(int num)
      { setInteger<int>(num); }

    /// Set this this tntdb::Decimal object to the value of the given long.
    /// @param num integer value to set this Decimal number to.
    void setLong(long num)
      { setInteger<long>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int32_t.
    /// @param num integer value to set this Decimal number to.
    void setInt32(int32_t num)
      { setInteger<int32_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given unsigned.
    /// @param num integer value to set this Decimal number to.
    void setUnsigned(unsigned num)
      { setInteger<unsigned>(num); }

    /// Set this this tntdb::Decimal object to the value of the given unsigned.
    /// @param num integer value to set this Decimal number to.
    void setUnsignedLong(unsigned long num)
      { setInteger<unsigned long>(num); }

    /// Set this this tntdb::Decimal object to the value of the given uint32_t.
    /// @param num integer value to set this Decimal number to.
    void setUnsigned32(uint32_t num)
      { setInteger<uint32_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int64_t.
    /// @param num integer value to set this Decimal number to.
    void setInt64(int64_t num)
      { setInteger<int64_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given uint64_t.
    /// @param num integer value to set this Decimal number to.
    void setUnsigned64(uint64_t num)
      { setInteger<int64_t>(num); }

    /// Set this this tntdb::Decimal object to the value of the given int64_t
    /// decimal mantissa and base 10 exponent.
    /// @param num decimal mantissa integer value to set this Decimal number to.
    /// @param exp base 10 exponent.
    void setDecimalInt64(int64_t num, int32_t exp)
      { setDecimalInteger<int64_t>(num, exp); }

    /// Set this this tntdb::Decimal object to the value of the given uint64_t
    /// decimal mantissa and base 10 exponent.
    /// @param num decimal mantissa integer value to set this Decimal number to.
    /// @param exp base 10 exponent.
    void setDecimalUnsigned64(uint64_t num, int32_t exp)
      { setDecimalInteger<int64_t>(num, exp); }

    /// Set this this tntdb::Decimal object to the value of the given float.
    /// @param num float value to set this Decimal number to.
    void setFloat(float num)
      { setFloatingPoint<float>(num); }

    /// Set this this tntdb::Decimal object to the value of the given double.
    /// @param num double value to set this Decimal number to.
    void setDouble(double num)
      { setFloatingPoint<double>(num); }
    
    /// Return this Decimal number as a string.
    /// @return string representation of this decimal number.
    std::string toString() const;

    /// Print this Decimal number.  If out.precision() != 0, then this
    /// decimal number is printed with out.precision() significant digits.
    /// @param out output stream
    std::ostream &print(std::ostream &out) const;

    /// Print this Decimal number.  If out.precision() != 0, then
    /// this decimal number is printed with out.precision() significant digits.
    /// @param out output stream
    /// @param printFlags the optional printFlags only affect
    /// how positive and negative infinity are printed.
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

    ///@name Low level arithmetic methods
    //\@{
    /// Multiply an integer type by 10, checking for overflow.
    /// @param n on input: the number to multply by 10, on output, the
    /// result of the multiplication by 10.
    /// @return true if overflow detected, else false.
    template <typename T>
    static bool overflowDetectedInMultiplyByTen(T &n);

    /// Divide an integer type by a power of 10 divisor, checking
    /// for overflow.
    /// @param dividend the integer dividend.
    /// @param quotient the result of the division by the power of 10 divisor.
    /// @param remainder the remainder result of the division by the power of 10 divisor.
    /// @param divisorPowerOfTenDigits divide by 10^divisorPowerOfTenDigits.
    /// @return true if overflow detected, else false.
    /// @throw std::overflow_error if the result will not fit
    template <typename ManType>
    static void divideByPowerOfTen(const ManType dividend,
                                   ManType &quotient,
                                   ManType &remainder,
                                   ManType divisorPowerOfTenDigits) throw(std::overflow_error);
    //\@}

    void normalize();

    bool operator== (const tntdb::Decimal& other) const;

    bool operator!= (const tntdb::Decimal& other) const
    { return !(*this == other); }

    bool operator< (const tntdb::Decimal& other) const;

    bool operator> (const tntdb::Decimal& other) const
    { return other < *this; }

    bool operator<= (const tntdb::Decimal& other) const
    { return !(other < *this); }

    bool operator>= (const tntdb::Decimal& other) const
    { return !(*this < other); }

  protected:
    ///@internal
    //\@{
    /// Initialize this Decimal number, called by the constructors.
    /// @param m integer decimal mantissa value to set this Decimal number to.
    /// @param e integer base 10 exponent to set this Decimal number to.
    /// @param f the flags, need to specify positive or negative. @link Decimal::FlagsTypeEnum @endlink
    /// @param pf the print flags for infinity and not a number. @link Decimal::InfinityOutputType @endlink
    void init(MantissaType m, ExponentType e, FlagsType f = positive, PrintFlagsType pf = infinityShort);

    /// Print fraction
    static void printFraction(std::ostream &out,
                              ExponentType fracDigits,
                              MantissaType fractional);
    //\@}
  };

  template <typename T>
  bool Decimal::overflowDetectedInMultiplyByTen(T &n)
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
      overflowDetected = Decimal::overflowDetectedInMultiplyByTen(multiplier);
    }
    return noDigits;
  }

  template <typename ManType>
  void Decimal::divideByPowerOfTen(const ManType dividend,
                                   ManType &quotient,
                                   ManType &remainder,
                                   ManType divisorPowerOfTenDigits) throw(std::overflow_error)
  {
    ManType divisorExponentPowerOfTenDigitsRemaining = divisorPowerOfTenDigits;
    // If the divisorPowerOfTenDigits is larger than
    // maxDivisorDigits, then do 2 (or more if required) divides
    // to avoid integer overflow in the multiplication calculating
    // the divisor.  The largest mulitple of 10 that can be stored in an
    // unsigned 64 bit int is 10^19, signed 64 bit int is 10^18,
    // 32 bit signed or unsigned int is 10^9, 16 bit signed or unsigned int
    // is 10000, 8 bit signed or unsigned char is 100.
    const ManType maxDivisorDigits = ((sizeof(ManType) >= 8) ? ((ManType(-1) > 0) ? 19 : 18) :
                                      ((sizeof(ManType) == 4) ? 9 : ((sizeof(ManType) == 2) ? 4 : 2 )));
    ManType exponentDivisor = ManType(Base);
    ManType previousExponentDivisor = ManType(Base);
    bool overflowDetected = false;
    if (divisorExponentPowerOfTenDigitsRemaining > maxDivisorDigits)
    {
      // The first divide is to make divisorExponentPowerOfTenDigitsRemaining an even multiple of
      // maxDivisorDigits.
      ManType divideChunksRemaining = divisorExponentPowerOfTenDigitsRemaining / maxDivisorDigits;
      ManType firstDivideDigits = divisorExponentPowerOfTenDigitsRemaining % maxDivisorDigits;
      for (ManType i = 1; (i < firstDivideDigits) && !overflowDetected; ++i)
      {
        previousExponentDivisor = exponentDivisor;
        overflowDetected = overflowDetectedInMultiplyByTen(exponentDivisor);
      }
      if (overflowDetected)
        throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::divideByPowerOfTen()"));
      quotient = ManType(dividend) / exponentDivisor;
      divisorExponentPowerOfTenDigitsRemaining -= firstDivideDigits;
      --divideChunksRemaining;
      exponentDivisor = ManType(Base);
      // If divisorExponentPowerOfTenDigitsRemaining is still larger than maxDivisorDigits,
      // then do more divides until it is equal to maxDivisorDigits.
      if (divideChunksRemaining > 0)
      {
        // Calculate the 10^maxDivisorDigits divisor
        for (ManType i = 1; (i < maxDivisorDigits) && !overflowDetected; ++i)
        {
          previousExponentDivisor = exponentDivisor;
          overflowDetected = overflowDetectedInMultiplyByTen(exponentDivisor);
        }
        if (overflowDetected)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::divideByPowerOfTen()"));
        // Do all but the last divide
        while (divideChunksRemaining > 0)
        {
          quotient = ManType(dividend) / exponentDivisor;
          --divideChunksRemaining;
          divisorExponentPowerOfTenDigitsRemaining -= maxDivisorDigits;
        }
        exponentDivisor = ManType(Base);
      }
    }
    for (ManType i = 1; (i < divisorExponentPowerOfTenDigitsRemaining) && !overflowDetected; ++i)
    {
      previousExponentDivisor = exponentDivisor;
      overflowDetected = overflowDetectedInMultiplyByTen(exponentDivisor);
    }
    if (overflowDetected)
      throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::divideByPowerOfTen()"));
    quotient = ManType(dividend) / exponentDivisor;
    remainder = ManType(dividend) % exponentDivisor;
  }

  template <typename ManType>
  void Decimal::getIntegralFractionalExponent(ManType &integral,
                                              ManType &fractional,
                                              ExponentType &ex,
                                              ExponentType optionalUserSpecifiedExponentOffset) const throw(std::overflow_error)
  {
    MantissaType integralPart = mantissa;
    MantissaType fractionalPart = MantissaType(0);
    ExponentType exp = exponent - optionalUserSpecifiedExponentOffset;
    if ((optionalUserSpecifiedExponentOffset != 0) && (integralPart != 0))
    {
      if (optionalUserSpecifiedExponentOffset >= 0)
      {
        MantissaType previousIntegralPart = MantissaType(0);
        bool overflowDetected = false;
        for (ExponentType i = 0; (i < optionalUserSpecifiedExponentOffset) && !overflowDetected; ++i)
        {
          previousIntegralPart = integralPart;
          overflowDetected = overflowDetectedInMultiplyByTen(integralPart);
        }
        if (overflowDetected)
          throw std::overflow_error(std::string("integer multiply overflow detected in Decimal::getIntegralFractionalExponent()"));
      }
      else
      {
        MantissaType absOptionalUserSpecifiedExponentOffset = MantissaType(-optionalUserSpecifiedExponentOffset);
        Decimal::divideByPowerOfTen(mantissa, integralPart, fractionalPart, absOptionalUserSpecifiedExponentOffset);
      }
    }
    ManType integralResult = 0;
    // if positive, or ManType is an unsigned integer type
    if (flags & positive)
    {
      integralResult = ManType(integralPart);
      // If the result is negative, or if the result after the cast
      // is different, then throw an overflow_error exception.
      if ((integralResult < 0) || (MantissaType(integralResult) != integralPart))
        throw std::overflow_error(std::string("integer overflow detected in Decimal::getIntegralFractionalExponent()"));
    }
    else
    {
      integralResult = -ManType(integralPart);
      // If ManType is an unsigned integer type, or if the
      // result is positive, or if the absolute value of the result
      // after the cast is different, then throw an overflow_error exception.
      if ((ManType(-1) > ManType(0)) || (integralResult > 0) || (MantissaType(-integralResult) != integralPart))
        throw std::overflow_error(std::string("integer overflow detected in Decimal::getIntegralFractionalExponent()"));
    }
    integral = integralResult;
    fractional = ManType(fractionalPart);
    ex = exp;
  }

  template <typename IntegerType>
  IntegerType Decimal::getInteger(RoundingAlgorithmType roundingAlgorithm) const throw(std::overflow_error)
  {
    ExponentType exp = 0;
    IntegerType quotient = 0;
    IntegerType remainder = 0;
    IntegerType result = 0;
    getIntegralFractionalExponent<IntegerType>(quotient,
                                               remainder,
                                               exp,
                                               exponent);
    if (exp >= 0)
      return quotient;

    ExponentType absExponent = -exp;
    IntegerType oneHalf = IntegerType(Base / 2);
    for (ExponentType i = 1; i < absExponent; ++i)
    {
      oneHalf *= IntegerType(Base);
    }

    switch(roundingAlgorithm)
    {
      case round:
        // if positive, or IntegerType is an unsigned integer type
        if ((flags & positive) || (IntegerType(-1) > IntegerType(0)))
        {
          result = (remainder >= oneHalf) ? quotient + IntegerType(1) : quotient;
        }
        else
        {
          result = (remainder >= oneHalf) ? -(quotient + IntegerType(1)) : -quotient;
        }

      case bankersRound:
        if ((flags & positive) || (IntegerType(-1) > IntegerType(0)))
        {
          result = (remainder > oneHalf) ? quotient + IntegerType(1) : quotient;
        }
        else
        {
          result = (remainder > oneHalf) ? -(quotient + IntegerType(1)) : -quotient;
        }
      default:
        {
          // default is roundingAlgorithm = truncate
          result = ((flags & positive) || (IntegerType(-1) > IntegerType(0))) ? quotient : -quotient;
        }
    }
    return result;
  }

  template <typename FloatingPointType>
  FloatingPointType Decimal::getFloatingPoint() const
  {
    if (exponent >= 0)
    {
      FloatingPointType exponentMultiplier = FloatingPointType(1);
      for (ExponentType i = 0; i < exponent; ++i)
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
      for (ExponentType i = 1; i < absExponent; ++i)
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
