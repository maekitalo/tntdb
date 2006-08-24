/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

