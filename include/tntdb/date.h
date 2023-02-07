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

#ifndef TNTDB_DATE_H
#define TNTDB_DATE_H

#include <string>

namespace tntdb
{
/// This class holds a date.
class Date
{
private:
    unsigned short _year;
    unsigned short _month;
    unsigned short _day;

public:
    /// Create a Date object with empty values
    Date()
      : _year(0), _month(0), _day(0)
      { }

    /** Create a date object with the given values

        No range checks are done.
     */
    Date(unsigned short year,
         unsigned short month,
         unsigned short day)
      : _year(year),
        _month(month),
        _day(day)
        { }

    static Date localtime();
    static Date gmtime();

    /// Get the year part of the Date
    unsigned short getYear() const  { return _year; }
    /// Get the month part of the Date
    unsigned short getMonth() const { return _month; }
    /// Get the day part of the Date
    unsigned short getDay() const   { return _day; }
    /// Get the day of week of the Date (from 0 = sunday to 6 = saturday)
    unsigned short getWDay() const;

    bool isNull() const  { return _month == 0; }

    /// Set the date
    void set(unsigned short year,
             unsigned short month,
             unsigned short day)
    {
        _year = year;
        _month = month;
        _day = day;
    }

    /// Get the date in ISO format (yyyy-mm-dd)
    std::string getIso() const;

    /** Construct a Date object from the given ISO date string

        If the string is not in ISO format, an exception of type tntdb::TypeError is thrown.
     */
    static Date fromIso(const std::string& s);

    bool operator== (const Date& dt) const
    {
        return _year == dt._year
          && _month  == dt._month
          && _day    == dt._day;
    }

    bool operator!= (const Date& dt) const
      { return !operator==(dt); }

    bool operator< (const Date& dt) const
    {
        return _year < dt._year  ? true
          : _year    > dt._year  ? false
          : _month   < dt._month ? true
          : _month   > dt._month ? false
          : _day     < dt._day;
    }

    bool operator> (const Date& dt) const
      { return dt < *this; }

    bool operator<= (const Date& dt) const
      { return !(*this > dt); }

    bool operator>= (const Date& dt) const
      { return !(*this < dt); }
};
}

#endif // TNTDB_DATE_H

