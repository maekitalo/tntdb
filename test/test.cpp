/*
 * Copyright (C) 2007 Tommi Maekitalo
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


#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/result.h>
#include <tntdb/value.h>
#include <tntdb/statement.h>
#include <tntdb/blob.h>
#include <tntdb/error.h>
#include <cxxtools/loginit.h>
#include <cxxtools/hdstream.h>
#include <cxxtools/arg.h>

class Tester
{
    tntdb::Connection conn;
    bool keep;

  public:
    Tester(const std::string& dburl)
      : conn(tntdb::connect(dburl)),
        keep(false)
      { }

    void setKeep(bool sw = true)   { keep = sw; }
    bool isKeep() const            { return keep; }

    void createTables(bool postgresql);
    void insertDataStmt();
    void testSelect();
    void testSelectRow();
    void testSelectValue();
    void testSelectByName();
    void testSelectRowByName();
    void testCursor();
    void testCursorByName();
    void testBlob();
    void dropTables();

    void test(bool postgresql);
};

void Tester::createTables(bool postgresql)
{
  std::cout << "create tables" << std::endl;

  try
  {
    if (postgresql)
      conn.execute(
        "create table tab1 ("
        "  a integer not null primary key,"
        "  b bytea not null)" );
    else
      conn.execute(
        "create table tab1 ("
        "  a integer not null primary key,"
        "  b varchar(255) not null)" );

    std::cout << "table tab1 created" << std::endl;
  }
  catch (const tntdb::Error& e)
  {
    std::cerr << "create table failed with error message: " << e.what() << std::endl;
  }
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
    std::cout << "a=" << a << "\tb=" << b << " len(b)=" << b.size() << std::endl;
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

void Tester::testBlob()
{
  std::cout << "blob insert" << std::endl;
  tntdb::Statement stmt = conn.prepare(
    "insert into tab1 (a, b)"
    "  values (:a, :b)");

  const char* bindata = "\1\2\3\0abc";
  tntdb::Blob blob1(bindata, 7);

  std::cout << "blob1.size()=" << blob1.size() << std::endl;

  stmt.set("a", 10)
      .set("b", blob1)
      .execute();

  cxxtools::Hdostream o;

  std::cout << "blob select" << std::endl;
  tntdb::Blob blob2 = conn.selectValue("select b from tab1 where a = 10").getBlob();

  o << std::string(blob2.data(), blob2.size()) << std::flush;

  std::string blobData1(blob1.data(), blob1.size());
  std::string blobData2(blob2.data(), blob2.size());
  std::cout << "blob1==blob2: " << (blobData1 == blobData2) << " size=" << blobData2.size() << std::endl;
  if (blobData1 != blobData2)
    std::cerr << "blob failed" << std::endl;
}

void Tester::dropTables()
{
  std::cout << "drop table" << std::endl;
  conn.execute("drop table tab1");
  std::cout << "table tab1 dropped" << std::endl;
}

void Tester::test(bool postgresql)
{
  createTables(postgresql);
  insertDataStmt();
  testSelect();
  testSelectRow();
  testSelectValue();
  testSelectByName();
  testSelectRowByName();
  testCursor();
  testCursorByName();
  testBlob();
  if (!keep)
    dropTables();
  else
    std::cout << "drop tables skipped" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> keep(argc, argv, 'k');
    cxxtools::Arg<bool> droponly(argc, argv, 'd');
    cxxtools::Arg<bool> postgresql(argc, argv, 'p');

    log_init();

    if (argc < 2)
    {
      std::cerr << "usage: " << argv[0] << " dburl" << std::endl;
      return -1;
    }

    Tester tester(argv[1]);
    tester.setKeep(keep);
    if (droponly)
      tester.dropTables();
    else
      tester.test(postgresql);
    std::cout << "success" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

