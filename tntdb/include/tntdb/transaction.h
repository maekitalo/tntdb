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

#ifndef TNTDB_TRANSACTION_H
#define TNTDB_TRANSACTION_H

#include <tntdb/connection.h>
#include <cxxtools/noncopyable.h>

namespace tntdb
{
  /**
   * The class Transaction monitors the state of a transaction on a database-conection.
   *
   * The constructor starts by default a transaction on the database. The transactionstate
   * is hold it the class. The destructor rolls the transaction back, when not explicitely
   * commited or rolled back.
   */
  class Transaction : private cxxtools::NonCopyable
  {
      Connection db;
      bool active;

    public:
      /**
       * Start a transaction when instructed.
       */
      Transaction(const Connection& db_, bool starttransaction = true);

      /**
       * roll back transaction when still active
       */
      ~Transaction();

      const Connection& getConnection() const  { return db; }

      /**
       * Starts a new transaction. If there is a active transaction, it is
       * rolled back first
       */
      void begin();

      /**
       * commits the current transaction. If there is no active transaction,
       * then nothing is done. The transactionstate is reset.
       */
      void commit();

      /**
       * rolls back the current transaction. If there is no active transaction,
       * then nothing is done. The transactionstate is reset.
       */
      void rollback();
  };
}

#endif // TNTDB_TRANSACTION_H

