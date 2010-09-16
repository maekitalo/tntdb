/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <tntdb/transaction.h>
#include <cxxtools/log.h>

log_define("tntdb.transaction")

namespace tntdb
{
  Transaction::Transaction(const Connection& db_, bool starttransaction)
    : db(db_),
      active(false)
  {
    if (starttransaction)
      begin();
  }

  Transaction::~Transaction()
  {
    if (active)
    {
      try
      {
        rollback();
      }
      catch (const std::exception& e)
      {
      }
    }
  }

  void Transaction::begin()
  {
    if (active)
    {
      log_warn("transaction already active in begin - rollback first");
      rollback();
    }

    log_debug("begin transaction");
    db.beginTransaction();
    active = true;
  }

  void Transaction::commit()
  {
    if (active)
    {
      log_debug("commit transaction");
      db.commitTransaction();
      active = false;
    }
    else
    {
      log_warn("transaction not active in commit");
    }
  }

  void Transaction::rollback()
  {
    if (active)
    {
      log_debug("rollback transaction");
      db.rollbackTransaction();
      active = false;
    }
    else
    {
      log_warn("transaction not active in rollback");
    }
  }
}
