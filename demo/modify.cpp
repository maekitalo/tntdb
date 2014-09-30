/*
 * Copyright (C) 2014 Tommi Maekitalo
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


/*

 This demonstrates how to modify data in a database with tntdb.

 */

#include <iostream>
#include <cxxtools/log.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/transaction.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();  // logging should be initialized always

    if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " dburl" << std::endl;
      return -1;
    }

    const std::string database = argv[1];
    tntdb::Connection db = tntdb::connect(database);

    // insert
    tntdb::Statement ins = db.prepare("insert into foo(a, b)"
                                      " values (:a, :b)");
    ins.set("a", 42)
       .set("b", "Hi there")
       .execute();

    // update
    tntdb::Statement upd = db.prepare("update foo"
                                      "   set b = :b"
                                      " where a = :a");
    upd.set("a", 42)
       .set("b", "Hello")
       .execute();

    // delete
    tntdb::Statement del = db.prepare("delete from foo"
                                      " where a = :a");
    upd.set("a", 42)
       .execute();

    // using a transaction
    {
      // We define the transaction object in a scope.
      //
      // The transaction will be rolled back automatically in case the
      // end of the block is reached and no commit or rollback is called.
      // This happens typically when a exception is thrown.
      //
      // Note that when the rollback fails in the destructor errors are lost.

      tntdb::Transaction transaction(db);
      tntdb::Statement ins = db.prepare("insert into foo(a, b)"
                                        " values (:a, :b)");
      ins.set("a", 42)
         .set("b", "Hi there")
         .execute();

      ins.set("a", 43)
         .set("b", "Hello")
         .execute();

      transaction.commit();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

