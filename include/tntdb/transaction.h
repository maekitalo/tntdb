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

namespace tntdb
{
/** The class Transaction monitors the state of a transaction on a database connection

    The constructor by default starts a transaction on the database. The transaction state
    is held in the class. The destructor rolls back the transaction if it isn't explicitely
    commited or rolled back.
 */
class Transaction
{
    IConnection& _db;
    bool _active;

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

public:
    explicit Transaction(IConnection& db, bool starttransaction = true);

    /// Create a %Transaction object
    explicit Transaction(const Connection& db, bool starttransaction = true)
        : Transaction(*db.getImpl(), starttransaction)
        { }

    /// Roll back transaction if still active
    ~Transaction();

    /// Start a new transaction. If there is an active transaction, it is rolled back first
    void begin();

    /** Commit the current transaction.
    
        If there is no active transaction, nothing is done. The transaction state is reset.
     */
    void commit();

    /** Roll back the current transaction

        If there is no active transaction, nothing is done. The transaction state is reset.
     */
    void rollback();

    /** Lock the specified table

        Locks are released when the transaction is committed or rolled back
        either explicitly or implicitly by the destructor.
     */
    void lockTable(const std::string& tableName, bool exclusive = true);
};
}

#endif // TNTDB_TRANSACTION_H

