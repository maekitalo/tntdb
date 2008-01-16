/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef TNTDB_IFACE_ICONNECTIONMANAGER_H
#define TNTDB_IFACE_ICONNECTIONMANAGER_H

#include <tntdb/noncopyable.h>
#include <string>

#define TNTDB_DRIVER_PRAEFIX connectionManager1_

#define TNTDB_CONNECTIONMANAGER_DECLARE(drivername) \
  extern "C" { extern tntdb::drivername::ConnectionManager connectionManager1_ ## drivername; }

#define TNTDB_CONNECTIONMANAGER_DEFINE(drivername) \
  extern "C" { tntdb::drivername::ConnectionManager connectionManager1_ ## drivername; }

namespace tntdb
{
  class Connection;

  class IConnectionManager : private NonCopyable
  {
    public:
      virtual ~IConnectionManager() { }
      virtual Connection connect(const std::string& url) = 0;
  };
}

#endif // TNTDB_IFACE_ICONNECTIONMANAGER_H

