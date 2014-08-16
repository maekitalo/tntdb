/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef TNTDB_TIME_H
#define TNTDB_TIME_H

#include <string>

namespace tntdb
{
  /// This class holds a time.
  class Time
  {
    private:
      short _hour;
      unsigned short _minute;
      unsigned short _second;
      unsigned short _millis;

    public:
      /// Create a Time object with empty values
      Time()
        : _hour(-1),
          _minute(0),
          _second(0),
          _millis(0)
          { }

      /** Create a Time object with the given values

          No range checks are done.
       */
      Time(unsigned short hour,
           unsigned short minute,
           unsigned short second,
           unsigned short millis = 0)
        : _hour(hour),
          _minute(minute),
          _second(second),
          _millis(millis)
          { }

      static Time localtime();
      static Time gmtime();

      /// Get the hour part of the Time
      unsigned short getHour() const   { return static_cast<unsigned short>(_hour); }
      /// Get the minute part of the Time
      unsigned short getMinute() const { return _minute; }
      /// Get the second part of the Time
      unsigned short getSecond() const { return _second; }
      /// Get the millisecond part of the Time
      unsigned short getMillis() const { return _millis; }

      bool isNull() const  { return _hour < 0; }

      /** Sets the time

          No range checks are done
       */
      void set(unsigned short hour,
               unsigned short minute,
               unsigned short second,
               unsigned short millis = 0)
      {
        _hour = static_cast<short>(hour);
        _minute = minute;
        _second = second;
        _millis = millis;
      }

      /// Get the time in ISO format (hh:mm:ss.hhh)
      std::string getIso() const;

      /** Construct a Time object from the given ISO date string

          If the string is not in ISO format, an exception of type tntdb::TypeError is thrown.
       */
      static Time fromIso(const std::string& s);

      bool operator== (const Time& dt) const
      {
        return _hour == dt._hour
          && _minute == dt._minute
          && _second == dt._second
          && _millis == dt._millis;
      }

      bool operator!= (const Time& dt) const
        { return !operator==(dt); }

      bool operator< (const Time& dt) const
      {
        return _hour < dt._hour   ? true
          : _hour    > dt._hour   ? false
          : _minute  < dt._minute ? true
          : _minute  > dt._minute ? false
          : _second  < dt._second ? true
          : _second  > dt._second ? false
          : _millis  < dt._millis;
      }

      bool operator> (const Time& dt) const
        { return dt < *this; }

      bool operator<= (const Time& dt) const
        { return !(*this > dt); }

      bool operator>= (const Time& dt) const
        { return !(*this < dt); }
  };
}

#endif // TNTDB_TIME_H

