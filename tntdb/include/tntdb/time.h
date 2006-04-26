/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNTDB_TIME_H
#define TNTDB_TIME_H

#include <string>

namespace tntdb
{
  /**
   * This class holds a time.
   */
  class Time
  {
      short hour;
      unsigned short minute;
      unsigned short second;
      unsigned short millis;

    public:
      /// Initializes the Time-object with empty values.
      Time()
        : hour(-1),
          minute(0),
          second(0),
          millis(0)
          { }

      /// Initializes the Time-object with the given values.
      /// No range-checks are done.
      Time(unsigned short hour_,
           unsigned short minute_,
           unsigned short second_,
           unsigned short millis_ = 0)
        : hour(hour_),
          minute(minute_),
          second(second_),
          millis(millis_)
          { }

      /// Returns the hour-part of the Time.
      unsigned short getHour() const     { return static_cast<unsigned short>(hour); }
      /// Returns the minute-part of the Time.
      unsigned short getMinute() const   { return minute; }
      /// Returns the second-part of the Time.
      unsigned short getSecond() const   { return second; }
      /// Returns the millisecond-part of the Time.
      unsigned short getMillis() const   { return millis; }

      bool isNull() const  { return hour < 0; }

      /// Sets the time.
      /// No range-checks are done.
      void set(unsigned short hour_,
               unsigned short minute_,
               unsigned short second_,
               unsigned short millis_ = 0)
      {
        hour = static_cast<short>(hour_);
        minute = minute_;
        second = second_;
        millis = millis_;
      }

      /// Returns the time in ISO-format (hh:mm:ss.hhh)
      std::string getIso() const;
      /// Interprets the passed string as a time-string in ISO-format (hh:mm:ss.hhh) and returns a Time-object.
      /// When the string is not in ISO-format, a exception of type tntdb::TypeError is thrown.
      static Time fromIso(const std::string& s);

      bool operator== (const Time& dt) const
      { return hour == dt.hour
            && minute == dt.minute
            && second == dt.second
            && millis == dt.millis; }

      bool operator!= (const Time& dt) const
        { return !operator==(dt); }

      bool operator< (const Time& dt) const
      { return hour < dt.hour
            || hour == dt.hour
             && ( minute < dt.minute
              || minute == dt.minute
               && ( second < dt.second
                 || second == dt.second
                  && millis < dt.millis
                  )
                ); }

      bool operator> (const Time& dt) const
        { return dt < *this; }

      bool operator<= (const Time& dt) const
        { return !(*this > dt); }

      bool operator>= (const Time& dt) const
        { return !(*this < dt); }
  };
}

#endif // TNTDB_TIME_H

