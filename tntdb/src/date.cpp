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

#include <tntdb/date.h>
#include <tntdb/error.h>
#include <cctype>
#include <sys/time.h>
#include <time.h>
#include <string.h>

namespace tntdb
{
  Date Date::localtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    localtime_r(&t, &tm);

    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
  }

  Date Date::gmtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    gmtime_r(&t, &tm);

    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
  }

  unsigned short Date::getWDay() const
  {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;

    time_t t = mktime(&tm);
    localtime_r(&t, &tm);

    return tm.tm_wday;
  }


  std::string Date::getIso() const
  {
    // format YYYY-MM-DD
    //        0....+....1
    char ret[10];
    unsigned short n = year;
    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = '0' + month / 10;
    ret[6] = '0' + month % 10;
    ret[7] = '-';
    ret[8] = '0' + day / 10;
    ret[9] = '0' + day % 10;

    return std::string(ret, 10);
  }

  inline unsigned short getNumber2(const char* s)
  {
    if (!std::isdigit(s[0])
      || !std::isdigit(s[1]))
      throw TypeError();
    return (s[0] - '0') * 10
         + (s[1] - '0');
  }

  inline unsigned short getNumber4(const char* s)
  {
    if (!std::isdigit(s[0])
      || !std::isdigit(s[1])
      || !std::isdigit(s[2])
      || !std::isdigit(s[3]))
      throw TypeError();
    return (s[0] - '0') * 1000
         + (s[1] - '0') * 100
         + (s[2] - '0') * 10
         + (s[3] - '0');
  }

  Date Date::fromIso(const std::string& s)
  {
    if (s.size() < 10
      || s.at(4) != '-'
      || s.at(7) != '-')
      throw TypeError();
    const char* d = s.data();
    return Date(getNumber4(d), getNumber2(d + 5), getNumber2(d + 8));
  }

}
