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

#include <tntdb/datetime.h>
#include <tntdb/error.h>
#include <cctype>
#include <sys/time.h>
#include <time.h>

namespace tntdb
{
  Datetime Datetime::localtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    localtime_r(&t, &tm);

    return Datetime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec,
                    tv.tv_usec / 1000);
  }

  Datetime Datetime::gmtime()
  {
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, 0);
    time_t t = static_cast<time_t>(tv.tv_sec);
    gmtime_r(&t, &tm);

    return Datetime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec,
                    tv.tv_usec / 1000);
  }

  std::string Datetime::getIso() const
  {
    // format YYYY-MM-DD hh:mm:ss.sss
    //        0....+....1....+....2....+
    char ret[25];
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
    ret[10] = ' ';
    ret[11] = '0' + hour / 10;
    ret[12] = '0' + hour % 10;
    ret[13] = ':';
    ret[14] = '0' + minute / 10;
    ret[15] = '0' + minute % 10;
    ret[16] = ':';
    ret[17] = '0' + second / 10;
    ret[18] = '0' + second % 10;
    ret[19] = '.';
    n = millis;
    ret[22] = '0' + n % 10;
    n /= 10;
    ret[21] = '0' + n % 10;
    n /= 10;
    ret[20] = '0' + n % 10;

    return std::string(ret, 23);
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

  Datetime Datetime::fromIso(const std::string& s)
  {
    if (s.size() < 23
      || s.at(4) != '-'
      || s.at(7) != '-'
      || s.at(10) != ' '
      || s.at(13) != ':'
      || s.at(16) != ':'
      || s.at(19) != '.')
      throw TypeError();
    const char* d = s.data();
    return Datetime(getNumber4(d), getNumber2(d + 5), getNumber2(d + 8),
                    getNumber2(d + 11), getNumber2(d + 14), getNumber2(d + 17),
                    getNumber3(d + 20));
  }

}
