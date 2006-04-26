/* 
   Copyright (C) 2006 Tommi Maekitalo

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

#ifndef TNTDB_LIBRARYMANAGER_H
#define TNTDB_LIBRARYMANAGER_H

#include <tntdb/iface/iconnectionmanager.h>
#include <tntdb/bits/connection.h>
#include <cxxtools/dlloader.h>
#include <string>

namespace tntdb
{
  class LibraryManager
  {
      cxxtools::dl::Library lib;
      IConnectionManager* connectionManager;

    public:
      LibraryManager()
        : connectionManager(0)
        { }
      LibraryManager(const std::string& libname);
      Connection connect(const std::string url)
        { return connectionManager->connect(url); }
  };

}

#endif // TNTDB_LIBRARYMANAGER_H

