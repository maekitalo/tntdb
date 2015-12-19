/*
 * Copyright (C) 2015 Tommi Maekitalo
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

    This demo shows how to use the cxxtools serialization to output results
    from the database in various formats.

    In the header <tntdb/serialization.h> are proper operators for converting
    tntdb::Value, tntdb::Row, tntdb::Result or tntdb::Statement to a
    cxxtools::SerializationInfo, which is needed to use the serialization
    of cxxtools.

    By default the csv serializer is used but also json and xml output are
    implemented.

    Note that reading a tntdb::Statements executes the sql when iterating
    through the result.

 */

#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/serialization.h>

#include <cxxtools/csv.h>
#include <cxxtools/json.h>
#include <cxxtools/xml/xml.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();  // Logging should be initialized always; reads log.xml or log.properties to configure logging

        // parse command line switches -j and -x
        cxxtools::Arg<bool> json(argc, argv, 'j');
        cxxtools::Arg<bool> xml(argc, argv, 'x');

        if (argc <= 2)
        {
            std::cerr << "Usage: " << argv[0] << " [options] dburl \"select statement\"\n"
                         "Outputs the results of a select statement as csv or other format\n"
                         "\n"
                         "options:\n"
                         " -j     output as json\n"
                         " -x     output as xml\n"
                         "\n"
                         "e.g.:  " << argv[0] << " sqlite:mydata.db \"select * from foo\"\n";
            return -1;
        }

        // connect to database
        std::string database = argv[1];
        tntdb::Connection db = tntdb::connect(database);

        // prepare statement
        std::string selectStmt = argv[2];
        tntdb::Statement stmt = db.prepare(selectStmt);

        // output in requested format
        //
        // The functions cxxtools::Json, cxxtools::xml::Xml and cxxtools::Csv return
        // a object, which has a output operator, which outputs the data
        // to the requeste format. To extract the data, the deserialization operator
        // defined in <tntdb/serialization.h> are used to convert the database output
        // to a cxxtools::SerializationInfo, which is used to generate the output.
        //
        if (json)
        {
            std::cout << cxxtools::Json(stmt).beautify(true);
        }
        else if (xml)
        {
            std::cout << cxxtools::xml::Xml(stmt, "db").beautify(true);
        }
        else
        {
            std::cout << cxxtools::Csv(stmt);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
