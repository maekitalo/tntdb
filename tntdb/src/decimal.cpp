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

#include <tntdb/decimal.h>
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <cxxtools/convert.h>
#include <cxxtools/log.h>
#include <cmath>

log_define("tntdb.decimal")

namespace tntdb
{
  namespace
  {
    void throwConversionError(const std::string& s)
    {
      log_warn("failed to convert \"" << s << "\" to decimal");
      throw std::runtime_error("failed to convert \"" + s + "\" to decimal");
    }

    void throwOverflowError(const Decimal& d)
    {
      log_warn("overflow when trying to read integer from decimal " << d.toString());
      throw std::overflow_error("overflow when trying to read integer from decimal " + d.toString());
    }

    int stringCompareIgnoreCase(const char* const& s1, const char* const& s2)
    {
      const char* it1 = s1;
      const char* it2 = s2;
      while (*it1 && *it2)
      {
          if (*it1 != *it2)
          {
              char c1 = std::toupper(*it1);
              char c2 = std::toupper(*it2);
              if (c1 < c2)
                  return -1;
              else if (c2 < c1)
                  return 1;
          }
          ++it1;
          ++it2;
      }

      return *it1 ? 1
                  : *it2 ? -1 : 0;
    }

    void stripZeros(std::string& s)
    {
      std::string::size_type pos;
      for (pos = s.size(); pos > 1; --pos)
      {
        if (s[pos - 1] != '0')
          break;
      }
      s.erase(pos);
    }
  }

  class Decimal::Parser
  {
      enum {
        state_0,
        state_man0,
        state_man,
        state_fract0,
        state_fract,
        state_exp0,
        state_exp1,
        state_exp,
        state_special,
        state_end
      } _state;

      Decimal* _value;
      std::string _str;
      short _eoff;
      bool _eneg;

    public:
      Parser()
        : _value(0),
          _eoff(0),
          _eneg(false)
      { }

      void begin(Decimal& d);
      void parse(char ch);
      void finish();
  };
  
  void Decimal::Parser::begin(Decimal& d)
  {
    _state = state_0;
    _value = &d;
    _str.clear();
    _eoff = 0;
    _eneg = false;
    d._mantissa.clear();
    d._exponent = 0;
    d._negative = false;
  }

  void Decimal::Parser::parse(char ch)
  {
    _str += ch;
    switch (_state)
    {
      case state_0:
        if (ch == '+')
        {
          _state = state_man0;
        }
        else if (ch == '-')
        {
          _value->_negative = true;
          _state = state_man0;
        }
        else if (ch == '.')
        {
          _state = state_fract0;
        }
        else if (ch == '0')
        {
          _state = state_man;
        }
        else if (ch >= '1' && ch <= '9')
        {
          _value->_mantissa = ch;
          _state = state_man;
        }
        else if (std::isalpha(ch))
          _state = state_special;
        else if (!std::isspace(ch))
          throwConversionError(_str);
        break;

      case state_man0:
        if (ch == '.')
        {
          _state = state_fract0;
        }
        else if (ch == '0')
        {
          _state = state_man;
        }
        else if (ch >= '1' && ch <= '9')
        {
          _value->_mantissa = ch;
          _state = state_man;
        }
        else if (std::isalpha(ch))
          _state = state_special;
        else
          throwConversionError(_str);
        break;

      case state_man:
        if (ch == '0' && _value->_mantissa.empty())
        {
        }
        else if (ch >= '0' && ch <= '9')
        {
          _value->_mantissa += ch;
        }
        else
        {
          _eoff = _value->_mantissa.size();

          if (ch == '.')
          {
            _state = _value->_mantissa.empty() ? state_fract0 : state_fract;
          }
          else if (ch == 'e' || ch == 'E')
          {
            _state = state_exp0;
          }
          else if (std::isspace(ch))
            _state = state_end;
          else
            throwConversionError(_str);
        }
        break;

      case state_fract0:
        if (ch == '0')
        {
          --_eoff;
          break;
        }
        else if (ch == 'e' || ch == 'E')
        {
          if (_eoff == 0)
            throwConversionError(_str);

          _value->_mantissa = '0';
          _eoff = 0;
          _state = state_exp0;
          break;
        }

        _state = state_fract;
        // nobreak

      case state_fract:
        if (ch >= '0' && ch <= '9')
        {
          _value->_mantissa += ch;
        }
        else if (ch == 'e' || ch == 'E')
        {
          _state = state_exp0;
        }
        else if (std::isspace(ch))
          _state = state_end;
        else
          throwConversionError(_str);
        break;

      case state_exp0:
        if (ch == '+')
        {
          _state = state_exp1;
          break;
        }
        else if (ch == '-')
        {
          _eneg = true;
          break;
        }

        _state = state_exp1;

        // nobreak

      case state_exp1:
      case state_exp:
        if (ch >= '0' && ch <= '9')
        {
          if (_value->_exponent > (std::numeric_limits<short>::max() - (ch - '0') / 10))
            throw std::overflow_error("overflow error when converting \"" + _str + "\" to decimal");

          _value->_exponent = _value->_exponent * 10 + (ch - '0');
          _state = state_exp;
        }
        else if (std::isspace(ch))
          _state = state_end;
        else
          throwConversionError(_str);
        break;

      case state_special:
        if (_str.size() > 4)
          throwConversionError(_str);

        if (std::isalpha(ch) && _str.size() >= 3)
        {
          if (stringCompareIgnoreCase(_str.c_str(), "inf") == 0
           || stringCompareIgnoreCase(_str.c_str(), "+inf") == 0)
          {
            _value->_exponent = std::numeric_limits<short>::max();
            _state = state_end;
          }
          else if (stringCompareIgnoreCase(_str.c_str(), "-inf") == 0)
          {
            _value->_negative = true;
            _value->_exponent = std::numeric_limits<short>::max();
            _state = state_end;
          }
          else if (stringCompareIgnoreCase(_str.c_str(), "nan") == 0)
          {
            _value->_exponent = 0;
            _state = state_end;
          }
        }

        break;

      case state_end:
        if (!std::isspace(ch))
          throwConversionError(_str);
    }
  }

  void Decimal::Parser::finish()
  {
    if (_state == state_0 || _state == state_man0 || _state == state_exp0 || _state == state_exp1 || _state == state_special)
      throwConversionError(_str);

    if (_state == state_man)
    {
      if (_value->_mantissa.empty())
        _value->_mantissa = '0';
      else
        _eoff = _value->_mantissa.size();
    }

    if (_eneg)
      _value->_exponent = -_value->_exponent;

    _value->_exponent += _eoff;

    stripZeros(_value->_mantissa);
  }

  Decimal::Decimal()
    : _mantissa("0"),
      _exponent(0),
      _negative(false)
  {
  }

  Decimal::LongType Decimal::_getInteger(LongType min, LongType max) const
  {
    log_debug("getInteger; min=" << min << " max=" << max << " value=" << getDouble());

    if (!negative())
      return static_cast<LongType>(_getUnsigned(static_cast<UnsignedLongType>(max)));

    if (isPositiveInfinity() || isNegativeInfinity() || isNaN())
      throwOverflowError(*this);

    if (_exponent < 0)
      return 0;

    if (_exponent == 0)
      return !_mantissa.empty() && _mantissa[0] >= '5' ? -1 : 0;

    LongType ret = 0;
    std::string::size_type n;
    for (n = 0; _exponent - n > 0; ++n)
    {
      LongType d = n < _mantissa.size() ? (_mantissa[n] - '0') : 0;

      if (ret < (min + d) / 10)
        throwOverflowError(*this);

      ret = ret * 10 - d;
    }

    if (n < _mantissa.size() && _mantissa[n] >= '5')
    {
      if (ret == min)
        throwOverflowError(*this);
      --ret;
    }

    return ret;
  }

  Decimal::UnsignedLongType Decimal::_getUnsigned(UnsignedLongType max) const
  {
    log_debug("getUnsigned; max=" << max << " value=" << getDouble());

    if (negative() || isPositiveInfinity() || isNegativeInfinity() || isNaN())
      throwOverflowError(*this);

    if (_exponent < 0)
      return 0;

    if (_exponent == 0)
      return !_mantissa.empty() && _mantissa[0] >= '5' ? 1 : 0;

    UnsignedLongType ret = 0;
    std::string::size_type n;
    for (n = 0; _exponent - n > 0; ++n)
    {
      UnsignedLongType d = n < _mantissa.size() ? (_mantissa[n] - '0') : 0;

      if (ret > (max - d) / 10)
        throwOverflowError(*this);

      ret = ret * 10 + d;
    }

    if (n < _mantissa.size() && _mantissa[n] >= '5')
    {
      if (ret == max)
        throwOverflowError(*this);
      ++ret;
    }

    return ret;
  }

  void Decimal::setDouble(long double value)
  {
    if (value == std::numeric_limits<long double>::infinity())
    {
      _negative = false;
      _exponent = std::numeric_limits<short>::max();
    }
    else if (value == -std::numeric_limits<long double>::infinity())
    {
      _negative = true;
      _exponent = std::numeric_limits<short>::max();
    }
    else if (value != value) // check for nan
    {
      _negative = false;
      _exponent = 0;
    }
    else
    {
      long double v = value;
      _negative = v < 0;
      if (_negative)
        v = -v;

      _exponent = static_cast<short>(log10(v)) + 1;

      if (_exponent > std::numeric_limits<long double>::max_exponent10)
      {
        v /= 10;
        v /= std::pow(static_cast<long double>(10), static_cast<int>(_exponent - 1));
      }
      else if (_exponent < -std::numeric_limits<long double>::max_exponent10)
      {
        v *= 10;
        v /= std::pow(static_cast<long double>(10), static_cast<int>(_exponent + 1));
      }
      else
        v /= std::pow(static_cast<long double>(10), static_cast<int>(_exponent));

      for (int n = 0; n <= std::numeric_limits<long double>::digits10; ++n)
      {
        unsigned short d = static_cast<unsigned short>(v * 10);
        v = v * 10 - d;
        _mantissa += static_cast<char>(d + '0');
      }

      unsigned short d = static_cast<unsigned short>(v * 10);
      if (d >= 5)
        ++_mantissa[_mantissa.size()-1];

      stripZeros(_mantissa);
    }

    log_debug("double value=" << value << " => negative=" << _negative << " mantissa=" << _mantissa << " exponent=" << _exponent);
  }

  Decimal::Decimal(const std::string& value)
    : _exponent(0),
      _negative(false)
  {
    Parser parser;

    parser.begin(*this);

    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
      parser.parse(*it);

    parser.finish();

    log_debug("string value \"" << value << "\" => negative=" << _negative << " mantissa=" << _mantissa << " exponent=" << _exponent);
  }

  long double Decimal::getDouble() const
  {
    if (isPositiveInfinity())
      return std::numeric_limits<long double>::infinity();

    if (isNegativeInfinity())
      return -std::numeric_limits<long double>::infinity();

    if (isNaN())
      return std::numeric_limits<long double>::quiet_NaN();

    long double ret = 0;
    long double mul = 1;
    for (std::string::const_iterator it = _mantissa.begin(); it != _mantissa.end(); ++it)
    {
      mul /= 10;
      ret += (*it - '0') * mul;
    }

    if (_exponent == std::numeric_limits<long double>::max_exponent10 + 1)
    {
      ret *= std::pow(static_cast<long double>(10), static_cast<int>(_exponent - 1));
      ret *= 10;
      if (ret == std::numeric_limits<long double>::infinity())
        ret = std::numeric_limits<long double>::max();
    }
    else if (_exponent == std::numeric_limits<long double>::min_exponent10 - 1)
    {
      ret *= std::pow(static_cast<long double>(10), static_cast<int>(_exponent + 1));
      ret /= 10;
      if (ret == -std::numeric_limits<long double>::infinity())
        ret = std::numeric_limits<long double>::min();
    }
    else
    {
      ret *= std::pow(static_cast<long double>(10), static_cast<int>(_exponent));
    }

    if (_negative)
      ret = -ret;

    log_debug("getDouble mantissa=" << _mantissa << " exponent=" << _exponent << " negative=" << _negative << " => " << ret);

    return ret;
  }

  void Decimal::_setInteger(LongType l, short exponent)
  {
    _mantissa = cxxtools::convert<std::string>(l);

    if (_mantissa[0] == '-')
    {
      _mantissa.erase(0, 1);
      _negative = true;
    }
    else
      _negative = false;

    _exponent = exponent + _mantissa.size();
    stripZeros(_mantissa);
    log_debug("setInteger(" << l << ", " << exponent << ") => negative=" << _negative << " mantissa=" << _mantissa << " exponent=" << _exponent);
  }

  void Decimal::_setUnsigned(UnsignedLongType l, short exponent)
  {
    _mantissa = cxxtools::convert<std::string>(l);
    _negative = false;
    _exponent = exponent + _mantissa.size();
    stripZeros(_mantissa);
    log_debug("setUnsigned(" << l << ", " << exponent << ") => mantissa=" << _mantissa << " exponent=" << _exponent);
  }

  bool Decimal::operator< (const Decimal& other) const
  {
    if (_negative != other._negative)
      return _negative;

    if (_exponent != other._exponent)
      return _exponent < other._exponent;

    return _mantissa < other._mantissa;
  }

  std::string Decimal::toString() const
  {
    return (_exponent < 0 || _exponent > 8) ? toStringSci() : toStringFix();
  }

  std::string Decimal::toStringSci() const
  {
    if (isPositiveInfinity())
      return "inf";
    else if (isNegativeInfinity())
      return "-inf";
    else if (isNaN())
      return "nan";
    else
    {
      std::string ret;
      if (_negative)
        ret = '-';
      ret += _mantissa[0];
      if (_mantissa.size() > 1)
      {
        ret += '.';
        ret.append(_mantissa, 1, _mantissa.size() - 1);
      }

      ret += 'e';
      ret += cxxtools::convert<std::string>(_exponent - 1);

      return ret;
    }
  }

  std::string Decimal::toStringFix() const
  {
    if (isPositiveInfinity())
      return "inf";
    else if (isNegativeInfinity())
      return "-inf";
    else if (isNaN())
      return "nan";
    else
    {
      std::string ret;

      if (_exponent < 1)
      {
        ret = "0.";
        ret.append(-_exponent, '0');
        ret.append(_mantissa);
      }
      else
      {
        ret = _mantissa;
        if (_exponent < static_cast<short>(_mantissa.size()))
          ret.insert(_exponent, 1, '.');
        else
          ret.append(_exponent - _mantissa.size(), '0');
      }

      if (_negative)
        ret.insert(0u, 1u, '-');

      return ret;
    }
  }

  std::istream& operator>> (std::istream& in, Decimal& dec)
  {
    Decimal::Parser parser;
    parser.begin(dec);

    int ch;
    while ((ch = in.rdbuf()->sbumpc()) != std::istream::traits_type::eof() && !std::isspace(std::istream::traits_type::to_char_type(ch)))
    {
      parser.parse(std::istream::traits_type::to_char_type(ch));
    }

    parser.finish();

    return in;
  }

  std::ostream& operator<< (std::ostream& out, const Decimal& dec)
  {
    out << dec.toString();
    return out;
  }
}
// vim:et:sw=2
