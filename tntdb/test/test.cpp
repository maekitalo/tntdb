/* 
 * Copyright (C) 2007 Tommi Maekitalo
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


#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/result.h>
#include <tntdb/value.h>
#include <tntdb/statement.h>
#include <cxxtools/loginit.h>

class Tester
{
    tntdb::Connection conn;

  public:
    Tester(const std::string& dburl)
      : conn(tntdb::connect(dburl))
      { }

    void createTables();
    void insertDataStmt();
    void testSelect();
    void testSelectRow();
    void testSelectValue();
    void testSelectByName();
    void testSelectRowByName();
    void testCursor();
    void testCursorByName();
    void dropTables();

    void test();
};

void Tester::createTables()
{
  std::cout << "create tables" << std::endl;

  conn.execute(
    "create table tab1 ("
    "  a integer not null primary key,"
    "  b varchar(255) not null)" );

  std::cout << "table tab1 created" << std::endl;
}

void Tester::insertDataStmt()
{
  std::cout << "insert data" << std::endl;

  tntdb::Transaction trans(conn);

  tntdb::Statement stmt = conn.prepare(
    "insert into tab1 (a, b)"
    "  values (:a, :b)");

  stmt.set("a", 1)
      .set("b", "one")
      .execute();

  stmt.set("a", 2)
      .set("b", "two")
      .execute();

  stmt.set("a", 3)
      .set("b", "three")
      .execute();

  trans.commit();

  std::cout << "inserted data into tab1" << std::endl;
}

void Tester::testSelect()
{
  std::cout << "select data" << std::endl;

  tntdb::Result res = conn.select(
    "select a, b"
    "  from tab1"
    " order by a");

  for (tntdb::Result::const_iterator it = res.begin();
       it != res.end(); ++it)
  {
    int a = (*it)[0];
    std::string b = (*it)[1];
    std::cout << "a=" << a << "\tb=" << b << std::endl;
  }

  std::cout << "selected" << std::endl;
}

void Tester::testSelectRow()
{
  std::cout << "select row" << std::endl;

  tntdb::Row row = conn.selectRow(
    "select a, b"
    "  from tab1"
    " where a = (select max(a) from tab1)");

  int a = row[0];
  std::string b = row[1];
  std::cout << "a=" << a << "\tb=" << b << std::endl;
}

void Tester::testSelectValue()
{
  std::cout << "select value" << std::endl;

  tntdb::Value max = conn.selectValue(
    "select max(a) from tab1");

  std::cout << "max(a)=" << max.getInt() << std::endl;
}

void Tester::testSelectByName()
{
  std::cout << "select data by name" << std::endl;

  tntdb::Result res = conn.select(
    "select a, b"
    "  from tab1"
    " order by a");

  for (tntdb::Result::const_iterator it = res.begin();
       it != res.end(); ++it)
  {
    int a = (*it)["a"];
    std::string b = (*it)["b"];
    std::cout << "a=" << a << "\tb=" << b << std::endl;
  }

  std::cout << "selected" << std::endl;
}

void Tester::testSelectRowByName()
{
  std::cout << "select row by name" << std::endl;

  tntdb::Row row = conn.selectRow(
    "select a, b"
    "  from tab1"
    " where a = (select max(a) from tab1)");

  int a = row["a"];
  std::string b = row["b"];
  std::cout << "a=" << a << "\tb=" << b << std::endl;
}

void Tester::testCursor()
{
  std::cout << "select data with cursor" << std::endl;

  tntdb::Statement stmt = conn.prepare(
    "select a, b"
    "  from tab1"
    " where a > :value"
    " order by a");

  stmt.set("value", 1);

  for (tntdb::Statement::const_iterator it = stmt.begin();
       it != stmt.end(); ++it)
  {
    int a = (*it)[0];
    std::string b = (*it)[1];
    std::cout << "a=" << a << "\tb=" << b << std::endl;
  }

  std::cout << "selected with cursor" << std::endl;
}

void Tester::testCursorByName()
{
  std::cout << "select data by name with cursor" << std::endl;

  tntdb::Statement stmt = conn.prepare(
    "select a, b"
    "  from tab1"
    " where a > :value"
    " order by a");

  stmt.set("value", 1);

  for (tntdb::Statement::const_iterator it = stmt.begin();
       it != stmt.end(); ++it)
  {
    int a = (*it)["a"];
    std::string b = (*it)["b"];
    std::cout << "a=" << a << "\tb=" << b << std::endl;
  }

  std::cout << "selected with cursor" << std::endl;
}

void Tester::dropTables()
{
  std::cout << "drop table" << std::endl;
  conn.execute("drop table tab1");
  std::cout << "table tab1 dropped" << std::endl;
}

void Tester::test()
{
  createTables();
  insertDataStmt();
  testSelect();
  testSelectRow();
  testSelectValue();
  testSelectByName();
  testSelectRowByName();
  testCursor();
  testCursorByName();
  dropTables();
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    if (argc < 2)
    {
      std::cerr << "usage: " << argv[0] << " dburl" << std::endl;
      return -1;
    }

    Tester tester(argv[1]);
    tester.test();
    std::cout << "success" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

