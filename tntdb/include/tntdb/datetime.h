/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNTDB_DATETIME_H
#define TNTDB_DATETIME_H

#include <string>

namespace tntdb
{
  /**
   * This class holds a date and a time.
   */
  class Datetime
  {
      unsigned short year;
      unsigned short month;
      unsigned short day;
      unsigned short hour;
      unsigned short minute;
      unsigned short second;
      unsigned short millis;

    public:
      /// Initializes the Datetime-object with empty values.
      Datetime()
        : year(0),
          month(0),
          day(0),
          hour(0),
          minute(0),
          second(0),
          millis(0)
          { }

      /// Initializes the Datetime-object with the given values.
      /// No range-checks are done.
      Datetime(unsigned short year_,
               unsigned short month_,
               unsigned short day_,
               unsigned short hour_,
               unsigned short minute_,
               unsigned short second_,
               unsigned short millis_ = 0)
        : year(year_),
          month(month_),
          day(day_),
          hour(hour_),
          minute(minute_),
          second(second_),
          millis(millis_)
          { }

      /// Returns the year-part of the Datetime.
      unsigned short getYear() const     { return year; }
      /// Returns the month-part of the Datetime.
      unsigned short getMonth() const    { return month; }
      /// Returns the day-part of the Datetime.
      unsigned short getDay() const      { return day; }
      /// Returns the hour-part of the Datetime.
      unsigned short getHour() const     { return hour; }
      /// Returns the minute-part of the Datetime.
      unsigned short getMinute() const   { return minute; }
      /// Returns the second-part of the Datetime.
      unsigned short getSecond() const   { return second; }
      /// Returns the millisecond-part of the Datetime.
      unsigned short getMillis() const   { return millis; }

      bool isNull() const  { return month == 0; }

      /// Returns the date in ISO-format (yyyy-mm-dd hh:mm:ss.hhh)
      std::string getIso() const;
      /// Interprets the passed string as a datetime-string in ISO-format (yyyy-mm-dd hh:mm:ss.hhh) and returns a Datetime-object.
      /// When the string is not in ISO-format, a exception of type tntdb::TypeError is thrown.
      static Datetime fromIso(const std::string& s);

      /// Sets the date and the time.
      /// No range-checks are done.
      void set(unsigned short year_,
               unsigned short month_,
               unsigned short day_,
               unsigned short hour_,
               unsigned short minute_,
               unsigned short second_,
               unsigned short millis_ = 0)
      {
        year = year_;
        month = month_;
        day = day_;
        hour = hour_;
        minute = minute_;
        second = second_;
        millis = millis_;
      }

      bool operator== (const Datetime& dt) const
      { return year == dt.year
            && month == dt.month
            && day == dt.day
            && hour == dt.hour
            && minute == dt.minute
            && second == dt.second
            && millis == dt.millis; }

      bool operator!= (const Datetime& dt) const
        { return !operator==(dt); }

      bool operator< (const Datetime& dt) const
      { return year < dt.year
            || year == dt.year
             && ( month < dt.month
               || month == dt.month
                && ( day < dt.day
                  || day == dt.day
                   && ( hour < dt.hour
                     || hour == dt.hour
                      && ( minute < dt.minute
                        || minute == dt.minute
                         && ( second < dt.second
                           || second == dt.second
                            && millis < dt.millis
                            )
                         )
                       )
                    )
                ); }

      bool operator> (const Datetime& dt) const
        { return dt < *this; }

      bool operator<= (const Datetime& dt) const
        { return !(*this > dt); }

      bool operator>= (const Datetime& dt) const
        { return !(*this < dt); }
  };
}

#endif // TNTDB_DATETIME_H

