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

#ifndef TNTDB_DATETIME_H
#define TNTDB_DATETIME_H

#include <string>
#include <tntdb/date.h>
#include <tntdb/time.h>

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

      Datetime(const Date& date, const Time& time)
        : year(date.getYear()),
          month(date.getMonth()),
          day(date.getDay()),
          hour(time.getHour()),
          minute(time.getMinute()),
          second(time.getSecond()),
          millis(time.getMillis())
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

      static Datetime localtime();
      static Datetime gmtime();

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

      Date getDate() const  { return Date(year, month, day); }
      Time getTime() const  { return Time(hour, minute, second, millis); }

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
      {
        return year   < dt.year   ? true
             : year   > dt.year   ? false
             : month  < dt.month  ? true
             : month  > dt.month  ? false
             : day    < dt.day    ? true
             : day    > dt.day    ? false
             : hour   < dt.hour   ? true
             : hour   > dt.hour   ? false
             : minute < dt.minute ? true
             : minute > dt.minute ? false
             : second < dt.second ? true
             : second > dt.second ? false
             : millis < dt.millis;
      }

      bool operator> (const Datetime& dt) const
        { return dt < *this; }

      bool operator<= (const Datetime& dt) const
        { return !(*this > dt); }

      bool operator>= (const Datetime& dt) const
        { return !(*this < dt); }
  };
}

#endif // TNTDB_DATETIME_H

