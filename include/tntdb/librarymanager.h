/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef TNTDB_LIBRARYMANAGER_H
#define TNTDB_LIBRARYMANAGER_H

#include <tntdb/iface/iconnectionmanager.h>
#include <tntdb/bits/connection.h>
#include <cxxtools/library.h>
#include <string>
#include <vector>

namespace tntdb
{
class LibraryManager
{
    cxxtools::Library lib;
    IConnectionManager* connectionManager;

public:
    LibraryManager()
      : connectionManager(0)
      { }
    LibraryManager(const std::string& libname);
    Connection connect(const std::string& url, const std::string& username, const std::string& password)
      { return connectionManager->connect(url, username, password); }
};
}

#endif // TNTDB_LIBRARYMANAGER_H
