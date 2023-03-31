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
/// This class holds a date and a time
class Datetime
{
private:
    unsigned short _year;
    unsigned short _month;
    unsigned short _day;
    unsigned short _hour;
    unsigned short _minute;
    unsigned short _second;
    unsigned short _millis;

public:
    /// Create a Datetime object with empty values
    Datetime()
      : _year(0),
        _month(0),
        _day(0),
        _hour(0),
        _minute(0),
        _second(0),
        _millis(0)
        { }

    /// Create a Datetime object with the values from the given Date and Time objects
    Datetime(const Date& date, const Time& time)
      : _year(date.getYear()),
        _month(date.getMonth()),
        _day(date.getDay()),
        _hour(time.getHour()),
        _minute(time.getMinute()),
        _second(time.getSecond()),
        _millis(time.getMillis())
        { }

    /** Create a Datetime object with the given values

        No range checks are done.
     */
    Datetime(unsigned short year,
             unsigned short month,
             unsigned short day,
             unsigned short hour,
             unsigned short minute,
             unsigned short second,
             unsigned short millis = 0)
      : _year(year),
        _month(month),
        _day(day),
        _hour(hour),
        _minute(minute),
        _second(second),
        _millis(millis)
        { }

    static Datetime localtime();
    static Datetime gmtime();

    /// Get the year part of the Datetime
    unsigned short getYear() const   { return _year; }
    /// Get the month part of the Datetime
    unsigned short getMonth() const  { return _month; }
    /// Get the day part of the Datetime
    unsigned short getDay() const    { return _day; }
    /// Get the hour part of the Datetime
    unsigned short getHour() const   { return _hour; }
    /// Get the minute part of the Datetime
    unsigned short getMinute() const { return _minute; }
    /// Get the second part of the Datetime
    unsigned short getSecond() const { return _second; }
    /// Get the millisecond part of the Datetime
    unsigned short getMillis() const { return _millis; }

    Date getDate() const  { return Date(_year, _month, _day); }
    Time getTime() const  { return Time(_hour, _minute, _second, _millis); }

    bool isNull() const  { return _month == 0; }

    /// Get the date in ISO format (yyyy-mm-dd hh:mm:ss.hhh)
    std::string getIso() const;

    /** Construct a Datetime object from the given ISO datetime string

        If the string is not in ISO format, an exception of type tntdb::TypeError is thrown.
     */
    static Datetime fromIso(const std::string& s);

    /** Set the date and the time

        No range checks are done.
     */
    void set(unsigned short year,
             unsigned short month,
             unsigned short day,
             unsigned short hour,
             unsigned short minute,
             unsigned short second,
             unsigned short millis = 0)
    {
        _year = year;
        _month = month;
        _day = day;
        _hour = hour;
        _minute = minute;
        _second = second;
        _millis = millis;
    }

    bool operator== (const Datetime& dt) const
    {
        return _year == dt._year
          && _month  == dt._month
          && _day    == dt._day
          && _hour   == dt._hour
          && _minute == dt._minute
          && _second == dt._second
          && _millis == dt._millis;
    }

    bool operator!= (const Datetime& dt) const
      { return !operator==(dt); }

    bool operator< (const Datetime& dt) const
    {
        return _year < dt._year   ? true
          : _year    > dt._year   ? false
          : _month   < dt._month  ? true
          : _month   > dt._month  ? false
          : _day     < dt._day    ? true
          : _day     > dt._day    ? false
          : _hour    < dt._hour   ? true
          : _hour    > dt._hour   ? false
          : _minute  < dt._minute ? true
          : _minute  > dt._minute ? false
          : _second  < dt._second ? true
          : _second  > dt._second ? false
          : _millis  < dt._millis;
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

