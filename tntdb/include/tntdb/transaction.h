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

#ifndef TNTDB_TRANSACTION_H
#define TNTDB_TRANSACTION_H

#include <tntdb/connection.h>
#include <tntdb/noncopyable.h>

namespace tntdb
{
  /**
   * The class Transaction monitors the state of a transaction on a database-conection.
   *
   * The constructor starts by default a transaction on the database. The transactionstate
   * is hold it the class. The destructor rolls the transaction back, when not explicitely
   * commited or rolled back.
   */
  class Transaction : private NonCopyable
  {
      Connection db;
      bool active;

    public:
      /**
       * Start a transaction when instructed.
       */
      Transaction(const Connection& db_, bool starttransaction = true)
        : db(db_),
          active(false)
      {
        if (starttransaction)
          begin();
      }

      /**
       * roll back transaction when still active
       */
      ~Transaction()
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

      const Connection& getConnection() const  { return db; }

      /**
       * Starts a new transaction. If there is a active transaction, it is
       * rolled back first
       */
      void begin()
      {
        if (active)
          rollback();
        db.beginTransaction();
        active = true;
      }

      /**
       * commits the current transaction. If there is no active transaction,
       * then nothing is done. The transactionstate is reset.
       */
      void commit()
      {
        if (active)
        {
          db.commitTransaction();
          active = false;
        }
      }

      /**
       * rolls back the current transaction. If there is no active transaction,
       * then nothing is done. The transactionstate is reset.
       */
      void rollback()
      {
        if (active)
        {
          db.rollbackTransaction();
          active = false;
        }
      }
  };
}

#endif // TNTDB_TRANSACTION_H

