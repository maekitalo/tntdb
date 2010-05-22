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

#include <tntdb/decimal.h>
#include <sstream>
#include <iomanip>
#include <ios>
#include <limits>

namespace tntdb
{
  Decimal::Decimal()
    : mantissa(0),
      exponent(0),
      flags(positive),
      defaultPrintFlags(infinityShort)
  {
  }

  Decimal::Decimal(double value)
    : mantissa(0),
      exponent(0),
      flags(positive),
      defaultPrintFlags(infinityShort)
  {
    setDouble(value);
  }

  Decimal::Decimal(int64_t man, ExponentType exp)
    : mantissa(MantissaType(man >= int64_t(0) ? man : -man)),
      exponent(exp),
      flags(man >=int64_t(0) ? positive : 0),
      defaultPrintFlags(infinityShort)
  {
  }

  Decimal::Decimal(MantissaType man, ExponentType exp, FlagsType f, PrintFlagsType pf)
    : mantissa(man),
      exponent(exp),
      flags(f),
      defaultPrintFlags(pf)
  {
  }

  Decimal::MantissaType Decimal::getMantissa() const
  {
    return mantissa;
  }

  Decimal::ExponentType Decimal::getExponent() const
  {
    return exponent;
  }

  bool Decimal::isInfinity() const
  {
    return (flags & infinity);
  }

  bool Decimal::isInfinity(bool positiveInf) const
  {
    if (flags & infinity)
    {
      return ((flags & positive) == positiveInf);
    }
    else
      return false;
  }

  bool Decimal::isNaN() const
  {
    return (flags & NaN);
  }

  bool Decimal::isZero() const
  {
    if (flags & (infinity | NaN))
      return false;
    else
      return (mantissa == 0);
  }

  std::string Decimal::toString() const
  {
    std::ostringstream ostr;
    ostr.precision(24);
    print(ostr);
    return ostr.str();
  }

  void Decimal::printFraction(std::ostream &out,
                              ExponentType fracDigits,
                              MantissaType fractional)
  {
    // 2^64 = 18446744073709551616, which has 20 digits.
    const int maxDigits = 20;
    char buffer[maxDigits + 1];
    if (fracDigits > maxDigits)
      fracDigits = maxDigits;
    int digitsOutput = 0;
    int digitsProcessed = 0;
    MantissaType frac = fractional;
    buffer[maxDigits] = '\0';
    do
    {
      MantissaType fracQuotient = frac / MantissaType(Base);
      MantissaType fracRemainder = frac % MantissaType(Base);
      // Trailing zeros are not output
      if ((digitsOutput != 0) || (fracRemainder != 0))
        buffer[maxDigits - ++digitsOutput] = char(fracRemainder + '0');
      frac = fracQuotient;
    }
    while (++digitsProcessed < fracDigits);
    if (digitsOutput > 0)
      out << "." << &buffer[maxDigits - digitsOutput];
  }

  std::ostream &Decimal::print(std::ostream &out) const
  {
    return print(out, defaultPrintFlags);
  }

  std::ostream &Decimal::print(std::ostream &out, PrintFlagsType printFlags) const
  {
    int savePrecision = out.precision();
    std::ios::fmtflags saveFlags = out.flags(std::ios_base::left | std::ios_base::dec);
    if (flags & infinity)
    {
      if (flags & positive)
      {
        switch(printFlags)
        {
            case infinityTilde:
              out << "~";
              break;
            case infinityLong:
              out << "Infinity";
              break;
            default:
              out << "Inf";
              break;
        }
      }
      else
      {
        switch(printFlags)
        {
            case infinityTilde:
              out << "-~";
              break;
            case infinityLong:
              out << "-Infinity";
              break;
            default:
              out << "-Inf";
              break;
        }
      }
    }
    else
    {
      if (flags & NaN)
      {
        out << "NaN";
      }
      else
      {
        if (!(flags & positive))
        {
          out << "-";
        }
        else
        {
          if (saveFlags & std::ios_base::showpos)
          {
            out << "+";
          }
        }
        // Count number of mantissa digits
        ExponentType manDigits = ExponentType(numberOfDigits<MantissaType>(mantissa));
        ExponentType exp = 0;
        MantissaType integral = 0;
        MantissaType fractional = 0;
        ExponentType exponentOffset = 0;
        ExponentType fracDigits = 0;
        ExponentType precisionFracDigits = 0;
        if ((savePrecision != 0) && (manDigits > savePrecision))
        {
          // Then the number will not fit in precision digits without printing an exponent
          exponentOffset = 1 - manDigits;
          if (exponentOffset < 0)
            fracDigits = -exponentOffset;
          precisionFracDigits = savePrecision - 1;
        }
        else
        {
          exponentOffset = exponent;
          if (exponentOffset < 0)
            fracDigits = -exponentOffset;
          precisionFracDigits = fracDigits;
        }
        try
        {
          getIntegralFractionalExponent<MantissaType>(integral,
                                                      fractional,
                                                      exp,
                                                      ExponentType(exponentOffset));
          out << std::noshowpos << integral;
          if ((fracDigits > 0) && (precisionFracDigits > 0))
          {
            if (fracDigits > precisionFracDigits)
            {
              MantissaType precisionFractional = 0;
              MantissaType precisionFractionalRemainder = 0;
              MantissaType precisionFractionalDivisorDigits = MantissaType(fracDigits - precisionFracDigits);
              Decimal::divideByPowerOfTen(fractional,
                                          precisionFractional,
                                          precisionFractionalRemainder,
                                          precisionFractionalDivisorDigits);
              MantissaType oneHalf = MantissaType(Base / 2);
              bool overflowDetected = false;
              for (MantissaType j = 0; !overflowDetected && (j < precisionFractionalDivisorDigits - 1); ++j)
              {
                overflowDetected = Decimal::overflowDetectedInMultiplyByTen(oneHalf);
              }
              if (!overflowDetected && (oneHalf > 0) && (precisionFractionalRemainder >= oneHalf))
                ++precisionFractional;
              Decimal::printFraction(out, precisionFracDigits, precisionFractional);
            }
            else
            {
              Decimal::printFraction(out, fracDigits, fractional);
            }
          }
          if (exp != 0)
          {
            out << std::setw(0) << "e" << std::left << std::showpos << std::setw(0) << exp;
          }
        }
        catch(const std::overflow_error&)
        {
          out << std::noshowpos << mantissa;
          if (exponent != 0)
          {
            out << std::setw(0) << "e" << std::left << std::showpos << std::setw(0) << exponent;
          }
        }
      }
    }
    out.flags(saveFlags);
    return out;
  }

  std::ostream &operator<<(std::ostream& out, const Decimal& decimal)
  {
    return decimal.print(out);
  }

  void Decimal::normalize()
  {
    while (mantissa && mantissa % 10 == 0)
    {
      mantissa /= 10;
      ++exponent;
    }
  }

  bool Decimal::operator== (const tntdb::Decimal& other) const
  {
    tntdb::Decimal d0(*this);
    tntdb::Decimal d1(other);
    d0.normalize();
    d1.normalize();
    return d0.mantissa == d1.mantissa
        && d0.exponent == d1.exponent
        && d0.isPositive() == d1.isPositive();
  }

  bool Decimal::operator< (const tntdb::Decimal& other) const
  {
    tntdb::Decimal d0(*this);
    tntdb::Decimal d1(other);

    if (!d0.isPositive() && d1.isPositive())
      return true;

    if (d0.isPositive() && !d1.isPositive())
      return false;

    if( d0.exponent > d1.exponent)
    {
      while( d0.exponent != d1.exponent)
      {
        if( d0.mantissa > std::numeric_limits<MantissaType>::max()/10)
          return !d0.isPositive();

        d0.mantissa *= 10;
        --d0.exponent;
      }
    }
    else if( d0.exponent < d1.exponent)
    {
      while( d0.exponent != d1.exponent)
      {
        if( d1.mantissa > std::numeric_limits<MantissaType>::max()/10)
          return !d1.isPositive();

        d1.mantissa *= 10;
        --d1.exponent;
      }
    }

    if (d0.exponent < d1.exponent)
      return d0.isPositive();

    if (d0.exponent > d1.exponent)
      return !d0.isPositive();

    if (d0.mantissa < d1.mantissa)
      return d0.isPositive();

    if (d0.mantissa > d1.mantissa)
      return !d0.isPositive();

    return false;
  }

  void Decimal::init(MantissaType m, ExponentType e, FlagsType f, PrintFlagsType pf)
  {
    mantissa = m;
    exponent = e;
    flags = f;
    defaultPrintFlags = pf;
  }

  std::istream &Decimal::read(std::istream& in, bool ignoreOverflowReadingFraction)
  {
    enum DecimalReadStateEnum
    {
      start,
      readingMantissa,
      readingFraction,
      ignoringFraction,
      readingExponent,
      readingNaN,
      readingInfinity,
      finished,
      failed
    };
    DecimalReadStateEnum decimalReadState = start;
    const char infinityCharArray[] = "Infinity";
    int readIndex = 0;
    MantissaType mantissaMultiplier = 10;
    MantissaType man = 0;
    MantissaType previousMan = 0;
    typedef uint32_t UnsignedExponentType;
    UnsignedExponentType absExp = 0;
    UnsignedExponentType previousAbsExp = 0;
    ExponentType exponentSign = 1;
    ExponentType fracDigits = 0;
    FlagsType f = positive;
    PrintFlagsType pf = infinityShort;
    char readCharArray[sizeof(infinityCharArray)];
    int c = 0;
    for (c = in.peek(); (c != std::istream::traits_type::eof()) && (decimalReadState != finished) && (decimalReadState != failed); c = in.peek())
    {
      bool skipRead = false;
      switch (decimalReadState)
      {
          case start:
            switch (c)
            {
                case '-':
                  f &= ~positive;
                  decimalReadState = readingMantissa;
                  break;
                case '+':
                  decimalReadState = readingMantissa;
                  break;
                case '~':
                {
                  f |= infinity;
                  pf = infinityTilde;
                  decimalReadState = finished;
                }
                break;
                case 'I':
                case 'i':
                  readCharArray[readIndex++] = c;
                  decimalReadState = readingInfinity;
                  break;
                case 'N':
                  readCharArray[readIndex++] = c;
                  decimalReadState = readingInfinity;
                  break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  man = (man * mantissaMultiplier) + (c - '0');
                  decimalReadState = readingMantissa;
                  break;
                case 'e':
                case 'E':
                  decimalReadState = readingExponent;
                  break;
                case '.':
                  decimalReadState = readingFraction;
                  break;
                default:
                  skipRead = true;
                  decimalReadState = finished;
                  break;
            }
            break;
          case readingMantissa:
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  previousMan = man;
                  if (Decimal::overflowDetectedInMultiplyByTen(man) ||
                      ((man += (c - '0')) < previousMan))
                  {
                    // overflow detected
                    decimalReadState = failed;
                  }
                  break;
                case 'e':
                case 'E':
                  decimalReadState = readingExponent;
                  break;
                case '.':
                  decimalReadState = readingFraction;
                  break;
                default:
                  skipRead = true;
                  decimalReadState = finished;
                  break;
            }
            break;
          case readingFraction:
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  previousMan = man;
                  if (Decimal::overflowDetectedInMultiplyByTen(man) ||
                      ((man += (c - '0')) < previousMan))
                  {
                    // overflow detected
                    if (ignoreOverflowReadingFraction)
                    {
                      man = previousMan;
                      decimalReadState = ignoringFraction;
                    }
                    else
                    {
                      decimalReadState = failed;
                    }
                  }
                  else
                    fracDigits += 1;
                  break;
                case 'e':
                case 'E':
                  decimalReadState = readingExponent;
                  break;
                default:
                  skipRead = true;
                  decimalReadState = finished;
                  break;
            }
            break;
          case ignoringFraction:
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  break;
                case 'e':
                case 'E':
                  decimalReadState = readingExponent;
                  break;
                default:
                  skipRead = true;
                  decimalReadState = finished;
                  break;
            }
            break;
          case readingExponent:
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  previousAbsExp = absExp;
                  if (Decimal::overflowDetectedInMultiplyByTen(absExp) ||
                      ((absExp += (c - '0')) < previousAbsExp))
                  {
                    // overflow detected
                    decimalReadState = failed;
                  }
                  break;
                case '-':
                  exponentSign = -1;
                  break;
                case '+':
                  exponentSign = 1;
                  break;
                default:
                  skipRead = true;
                  decimalReadState = finished;
                  break;
            }
            break;
          case readingNaN:
            switch(readIndex)
            {
                case 1:
                  switch(c)
                  {
                      case 'a':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 2:
                  switch(c)
                  {
                      case 'N':
                        readCharArray[readIndex++] = c;
                        f = NaN;
                        decimalReadState = finished;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                default:
                  decimalReadState = failed;
                  break;
            }
            break;
          case readingInfinity:
            switch(readIndex)
            {
                case 1:
                  switch(c)
                  {
                      case 'n':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 2:
                  switch(c)
                  {
                      case 'f':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 3:
                  switch(c)
                  {
                      case 'i':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        // Accept Inf or -Inf, ignore anything following
                        // other than i.  If an i follows, we keep looking
                        // for Infinity or -Infinity
                        skipRead = true;
                        f |= infinity;
                        decimalReadState = finished;
                  }
                  break;
                case 4:
                  switch(c)
                  {
                      case 'n':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 5:
                  switch(c)
                  {
                      case 'i':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 6:
                  switch(c)
                  {
                      case 't':
                        readCharArray[readIndex++] = c;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                case 7:
                  switch(c)
                  {
                      case 'y':
                        readCharArray[readIndex++] = c;
                        f |= infinity;
                        decimalReadState = finished;
                        break;
                      default:
                        decimalReadState = failed;
                  }
                  break;
                default:
                  decimalReadState = failed;
                  break;
            }
            break;
      }
      if (!skipRead)
        c = in.get();
    }
    if ((c == std::istream::traits_type::eof()) || (decimalReadState == finished))
    {
      init(man, (ExponentType(absExp) * exponentSign) - fracDigits, f, pf);
    }
    else
    {
      in.setstate(std::ios_base::failbit);
    }
    return in;
  }

  std::istream &operator>>(std::istream& in, Decimal& decimal)
  {
    decimal.read(in);
    return in;
  }
}
