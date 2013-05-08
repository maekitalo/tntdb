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
  /**
   * This class holds a date.
   */
  class Date
  {
      unsigned short year;
      unsigned short month;
      unsigned short day;

    public:
      /// Initializes the Date-object with empty values.
      Date()
        : year(0), month(0), day(0)
        { }

      /// Initializes the date-object with the given values.
      /// No range-checks are done.
      Date(unsigned short year_,
           unsigned short month_,
           unsigned short day_)
        : year(year_),
          month(month_),
          day(day_)
          { }

      static Date localtime();
      static Date gmtime();

      /// Returns the year-part of the Date.
      unsigned short getYear() const     { return year; }
      /// Returns the month-part of the Date.
      unsigned short getMonth() const    { return month; }
      /// Returns the day-part of the Date.
      unsigned short getDay() const      { return day; }
      /// Returns the day of week of the Date (0=sunday to 6=saturday).
      unsigned short getWDay() const;

      bool isNull() const  { return month == 0; }

      /// sets the date.
      void set(unsigned short year_,
               unsigned short month_,
               unsigned short day_)
      {
        year = year_;
        month = month_;
        day = day_;
      }

      /// returns the date in ISO-format (yyyy-mm-dd)
      std::string getIso() const;
      /// interprets the passed string as a date-string in ISO-format (yyyy-mm-dd) and returns a Date-object.
      /// When the string is not in ISO-format, a exception of type tntdb::TypeError is thrown.
      static Date fromIso(const std::string& s);

      bool operator== (const Date& dt) const
      { return year == dt.year
            && month == dt.month
            && day == dt.day; }

      bool operator!= (const Date& dt) const
        { return !operator==(dt); }

      bool operator< (const Date& dt) const
      {
        return year   < dt.year   ? true
             : year   > dt.year   ? false
             : month  < dt.month  ? true
             : month  > dt.month  ? false
             : day    < dt.day;
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

