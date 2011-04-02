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

#include <tntdb/time.h>
#include <tntdb/error.h>
#include <cctype>
#include <sys/time.h>
#include <time.h>

namespace tntdb
{
  Time Time::localtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    localtime_r(&t, &tm);

    return Time(tm.tm_hour, tm.tm_min, tm.tm_sec,
                tv.tv_usec / 1000);
  }

  Time Time::gmtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    gmtime_r(&t, &tm);

    return Time(tm.tm_hour, tm.tm_min, tm.tm_sec,
                tv.tv_usec / 1000);
  }

  std::string Time::getIso() const
  {
    // format hh:mm:ss.sss
    //        0....+....1....+
    char ret[14];
    ret[0] = '0' + hour / 10;
    ret[1] = '0' + hour % 10;
    ret[2] = ':';
    ret[3] = '0' + minute / 10;
    ret[4] = '0' + minute % 10;
    ret[5] = ':';
    ret[6] = '0' + second / 10;
    ret[7] = '0' + second % 10;
    ret[8] = '.';
    unsigned short n = millis;
    ret[11] = '0' + n % 10;
    n /= 10;
    ret[10] = '0' + n % 10;
    n /= 10;
    ret[9] = '0' + n % 10;

    return std::string(ret, 12);
  }

  inline unsigned short getNumber2(const char* s)
  {
    if (!std::isdigit(s[0])
      || !std::isdigit(s[1]))
      throw TypeError();
    return (s[0] - '0') * 10
         + (s[1] - '0');
  }

  inline unsigned short getNumber3(const char* s)
  {
    if (!std::isdigit(s[0])
      || !std::isdigit(s[1])
      || !std::isdigit(s[2]))
      throw TypeError();
    return (s[0] - '0') * 100
         + (s[1] - '0') * 10
         + (s[2] - '0');
  }

  Time Time::fromIso(const std::string& s)
  {
    if (s.size() < 11
      || s.at(2) != ':'
      || s.at(5) != ':'
      || s.at(8) != '.')
      throw TypeError();
    const char* d = s.data();
    return Time(getNumber2(d), getNumber2(d + 3), getNumber2(d + 6),
                getNumber3(d + 9));
  }

}
