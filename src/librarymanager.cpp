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

#include <tntdb/librarymanager.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/directory.h>

#ifndef DRIVERDIR
#define DRIVERDIR "tntdb"
#endif

#define TNTDB_STRINGIFY(x) #x
#define TNTDB_TOSTRING(x) TNTDB_STRINGIFY(x)

log_define("tntdb.librarymanager")

namespace tntdb
{
  static const std::string libraryPrefix = "tntdb" ABI_CURRENT "-";

  LibraryManager::LibraryManager(const std::string& driverName)
  {
    try
    {
      log_debug("loading library \"" << libraryPrefix << driverName << '"');
      lib = cxxtools::Library(libraryPrefix + driverName);
    }
    catch (const cxxtools::FileNotFound& e)
    {
      log_debug("library \"" << libraryPrefix << driverName << "\" not found: " << e.what());
    }
    catch (const cxxtools::OpenLibraryFailed& e)
    {
      log_debug("opening library \"" << libraryPrefix << driverName << "\" failed: " << e.what());
    }

    try
    {
      if (!lib)
      {
        std::string d = DRIVERDIR;
        d += cxxtools::Directory::sep();
        d += libraryPrefix;
        d += driverName;
        log_debug("loading library \"" << d << '"');
        lib = cxxtools::Library(d);
      }
    }
    catch (const std::exception& e)
    {
      std::ostringstream msg;
      msg << "failed to load driver \"" << libraryPrefix << driverName << '"';
      log_warn(msg.str() << ": " << e.what());
      throw Error(msg.str());
    }

    std::string symbolName = TNTDB_TOSTRING(TNTDB_DRIVER_PRAEFIX) + driverName;
    void* sym = lib.getSymbol(symbolName.c_str());
    connectionManager = static_cast<IConnectionManager*>(sym);

    log_debug("driver " << driverName << " successfully loaded");
  }

}
