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

#include <tntdb/decimal.h>
#include <sstream>
#include <iomanip>

namespace tntdb
{
  Decimal::Decimal() :
    mantissa(0),
    exponent(0),
    flags(positive),
    defaultPrintFlags(infinityShort),
    fractionDigits(0)
  {
  }

  Decimal::Decimal(double value) :
    mantissa(0),
    exponent(0),
    flags(positive),
    defaultPrintFlags(infinityShort),
    fractionDigits(0)
  {
    setDouble(value);
  }

  Decimal::Decimal(int64_t man, ExponentType exp) :
    mantissa(MantissaType(man >= int64_t(0) ? man : -man)),
    exponent(exp),
    flags(man >=int64_t(0) ? positive : 0),
    defaultPrintFlags(infinityShort),
    fractionDigits(0)
  {
  }
  
  Decimal::Decimal(MantissaType man, ExponentType exp, FlagsType f, FractionDigitsType fd, PrintFlagsType pf) :
    mantissa(man),
    exponent(exp),
    flags(f),
    defaultPrintFlags(pf),
    fractionDigits(fd)
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
    return (flags & NaN)
  }
  
  bool Decimal::isZero() const
  {
    if (flags & (infinity | NaN))
      return false;
    else
      return (mantissa == 0)
  }

  std::string Decimal::toString() const
  {
    std::ostringstream ostr;
    print(ostr);
    return ostr.str();
  }

  std::ostream &Decimal::print(std::ostream &out) const
  {
    return print(out, defaultPrintFlags);
  }

  std::ostream &Decimal::print(std::ostream &out, PrintFlagsType printFlags) const
  {
    int saveFieldWidth = out.width(0);
    std::ios::fmtflags saveFlags = out.flags(std::ios_base::left | std::ios_base::dec);
    std::ios::char_type saveFill = out.fill(' ');
    printFormatted(out, printFlags);
    out.flags(saveFlags);
    out.width(saveFieldWidth);
    out.fill(saveFill);
    return out;
  }
  
  std::ostream &Decimal::printFormatted(std::ostream &out) const
  {
    return printFormatted(out, defaultPrintFlags);
  }
  
  std::ostream &Decimal::printFormatted(std::ostream &out, PrintFlagsType printFlags) const
  {
    int saveFieldWidth = out.width();
    std::ios::fmtflags saveFlags = out.flags();
    std::ios::char_type saveFill = out.fill(' ');
    int mantissaWidth = saveFieldWidth;
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
          if (mantissaWidth >= 0)
            --mantissaWidth;
        }
        else
        {
          if (saveFlags & std::ios_base::showpos)
          {
            out << "+";
            if (mantissaWidth >= 0)
              --mantissaWidth;
          }
        }
        if (fractionDigits != 0)
        {
          mantissaWidth -= (fractionDigits + 1);
        }
        if ((exponent != -fractionDigits) && (saveFieldWidth != 0))
        {
          // Count number of exponent digits 
          ExponentType expDigits = numberOfDigits<ExponentType>(exponent);
          // Subtract exponent characters from mantissa field width
          mantissaWidth -= (expDigits + 2);
        }
        if (mantissaWidth < 0)
          mantissaWidth = 0; 
        // Count number of mantissa digits 
        // ExponentType manDigits = ExponentType(numberOfDigits<MantissaType>(mantissa));
        ExponentType exp = 0;
        MantissaType integral = 0;
        MantissaType fractional = 0;
        if (fractionDigits == 0)
        {
          out << std::noshowpos << std::setw(mantissaWidth) << mantissa;
          if (exponent != 0)
          {
            out << std::setw(0) << "E" << std::left << std::showpos << std::setw(0) << exponent;
          }
        }
        else
        {
          try
          {
            getIntegralFractionalExponent<MantissaType>(integral,
                                                        fractional,
                                                        exp,
                                                        ExponentType(0));
            out << std::noshowpos << std::setw(mantissaWidth) << integral
                << "."
                << std::setw(fractionDigits) << std::setfill('0') << std::right << fractional;
          }
          catch(const std::overflow_error&)
          {
            out << std::noshowpos << std::setw(mantissaWidth) << mantissa;
            if (exponent != 0)
            {
              out << std::setw(0) << "E" << std::left << std::showpos << std::setw(0) << exponent;
            }
          }
        }
      }
    }
    out.flags(saveFlags);
    
    return out;
  }
    
  std::ostream &operator<<(std::ostream& out, const Decimal& decimal)
  {
    return decimal.printFormatted(out);
  }

  void Decimal::init(MantissaType m, ExponentType e, FlagsType f, FractionDigitsType fd, PrintFlagsType pf)
  {
    mantissa = m;
    exponent = e;
    flags = f;
    fractionDigits = fd;
    defaultPrintFlags = pf;
  }
  
  std::istream &Decimal::read(std::istream& in)
  {
    enum DecimalReadStateEnum
    {
      start,
      readingMantissa,
      readingFraction,
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
    ExponentType exponentMultiplier = 10;
    ExponentType exp = 0;
    ExponentType previousExp = 0;
    FractionDigitsType fracDigits = 0;
    FlagsType f = positive;
    PrintFlagsType pf = infinityShort;
    char readCharArray[sizeof(infinityCharArray)];
    bool skipRead = false;
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
                  previousMan = man;
                  man = (man * mantissaMultiplier) + (c - '0');
                  if (man < previousMan)
                  {
                    // overflow detected
                    decimalReadState = failed;
                  }
                  decimalReadState = readingMantissa;
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
                  man = (man * mantissaMultiplier) + (c - '0');
                  if (man < previousMan)
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
                  man = (man * mantissaMultiplier) + (c - '0');
                  if (man < previousMan)
                  {
                    // overflow detected
                    decimalReadState = failed;
                  }
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
                  previousExp = exp;
                  exp = (exp * exponentMultiplier) + (c - '0');
                  if (((exponentMultiplier > 0) && (exp < previousExp)) ||
                      ((exponentMultiplier < 0) && (exp > previousExp)))
                  {
                    // overflow detected
                    decimalReadState = failed;
                  }
                  break;
                case '-':
                  exponentMultiplier = -10;
                  break;
                case '+':
                  exponentMultiplier = 10;
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
      init(man, exp - fracDigits, f, fracDigits, pf);
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
