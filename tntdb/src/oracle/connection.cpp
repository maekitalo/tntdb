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

#include <tntdb/oracle/connection.h>
#include <tntdb/oracle/statement.h>
#include <tntdb/oracle/error.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>
#include <cxxtools/log.h>
#include <signal.h>

log_define("tntdb.oracle.connection")

#ifndef HAVE_SIGHANDLER_T
typedef void (*sighandler_t)(int);
#endif

namespace tntdb
{
  namespace oracle
  {
    namespace error
    {
      log_define("tntdb.oracle.error")

      void checkError(OCIError* errhp, sword ret, const char* function)
      {
        switch (ret)
        {
          case OCI_SUCCESS:
            break;

          case OCI_SUCCESS_WITH_INFO:
            log_warn(function << ": OCI_SUCCESS_WITH_INFO");
            break;

          case OCI_NEED_DATA:
            log_warn(function << ": OCI_NEED_DATA");
            throw Error(errhp, function);

          case OCI_NO_DATA:
            log_debug(function << ": OCI_NO_DATA");
            throw NotFound();

          case OCI_ERROR:
            throw Error(errhp, function);

          case OCI_INVALID_HANDLE:
            log_error("OCI_INVALID_HANDLE");
            throw InvalidHandle(function);

          case OCI_STILL_EXECUTING:
            log_error("OCI_STILL_EXECUTING");
            throw StillExecuting(function);

          case OCI_CONTINUE:
            log_error("OCI_CONTINUE");
            throw ErrorContinue(function);
        }
      }
    }

    void Connection::checkError(sword ret, const char* function) const
    {
      error::checkError(getErrorHandle(), ret, function);
    }

    namespace
    {
        class SighandlerSaver
        {
                int signum;
                sighandler_t sighandler;

            public:
                explicit SighandlerSaver(int signum_)
                    : signum(signum_)
                {
                    sighandler = signal(signum, SIG_DFL);
                }

                ~SighandlerSaver()
                {
                    signal(signum, sighandler);
                }
        };
    }

    void Connection::logon(const std::string& dblink, const std::string& user, const std::string& password)
    {
      log_debug("logon \"" << dblink << "\" user=\"" << user << '"');

      // workaround for OCI problem: OCI redirects SIGINT, which causes Ctrl-C to fail
      SighandlerSaver sighandlerSaver(SIGINT);

      sword ret;

      log_debug("create oracle environment");
      ret = OCIEnvCreate(&envhp, OCI_OBJECT, 0, 0, 0, 0, 0, 0);
      if (ret != OCI_SUCCESS)
        throw Error("cannot create environment handle");
      log_debug("environment handle => " << envhp);

      log_debug("allocate error handle");
      ret = OCIHandleAlloc(envhp, (void**)&errhp, OCI_HTYPE_ERROR, 0, 0);
      if (ret != OCI_SUCCESS)
        throw Error("cannot create error handle");
      log_debug("error handle => " << errhp);

      log_debug("allocate server handle");
      ret = OCIHandleAlloc(envhp, (void**)&srvhp, OCI_HTYPE_SERVER, 0, 0);
      checkError(ret, "OCIHandleAlloc(OCI_HTYPE_SERVER)");
      log_debug("server handle => " << srvhp);

      log_debug("allocate service handle");
      ret = OCIHandleAlloc(envhp, (void**)&svchp, OCI_HTYPE_SVCCTX, 0, 0);
      checkError(ret, "OCIHandleAlloc(OCI_HTYPE_SVCCTX)");
      log_debug("service handle => " << svchp);

      log_debug("attach to server");
      ret = OCIServerAttach(srvhp, errhp, reinterpret_cast<const text*>(dblink.data()), dblink.size(), 0);
      checkError(ret, "OCIServerAttach");

      /* set attribute server context in the service context */
      ret = OCIAttrSet(svchp, OCI_HTYPE_SVCCTX, srvhp, 0, OCI_ATTR_SERVER, errhp);
      checkError(ret, "OCIAttrSet(OCI_ATTR_SERVER)");

      log_debug("allocate session handle");
      ret = OCIHandleAlloc((dvoid*)envhp, (dvoid**)&usrhp, OCI_HTYPE_SESSION, 0, (dvoid**)0);
      checkError(ret, "OCIHandleAlloc(OCI_HTYPE_SESSION)");

      /* set username attribute in user session handle */
      log_debug("set username attribute in session handle");
      ret = OCIAttrSet(usrhp, OCI_HTYPE_SESSION,
        reinterpret_cast<void*>(const_cast<char*>(user.data())),
        user.size(), OCI_ATTR_USERNAME, errhp);
      checkError(ret, "OCIAttrSet(OCI_ATTR_USERNAME)");

      /* set password attribute in user session handle */
      ret = OCIAttrSet(usrhp, OCI_HTYPE_SESSION,
        reinterpret_cast<void*>(const_cast<char*>(password.data())),
        password.size(), OCI_ATTR_PASSWORD, errhp);
      checkError(ret, "OCIAttrSet(OCI_ATTR_PASSWORD)");

      /* start session */
      log_debug("start session");
      ret = OCISessionBegin(svchp, errhp, usrhp, OCI_CRED_RDBMS, OCI_DEFAULT);
      checkError(ret, "OCISessionBegin");

      /* set user session attrubte in the service context handle */
      ret = OCIAttrSet(svchp, OCI_HTYPE_SVCCTX, usrhp, 0, OCI_ATTR_SESSION, errhp);
      checkError(ret, "OCIAttrSet(OCI_ATTR_SESSION)");
    }

    namespace
    {
      std::string extractAttribute(std::string& value, const std::string& key)
      {
        std::string::size_type p0 = value.find(key);
        if (p0 == std::string::npos)
          return std::string();

        if (p0 > 0 && value.at(p0 - 1) != ';')
          return std::string();

        std::string::size_type p1 = value.find(';', p0);
        if (p1 == std::string::npos)
          p1 = value.size();
        std::string::size_type n = p1 - p0;

        std::string ret(value, p0 + key.size(), n - key.size());
        if (p0 > 0)
          value.erase(p0 - 1, n + 1);
        else
          value.erase(p0, n);
        return ret;
      }
    }

    Connection::Connection(const char* conninfo_)
      : envhp(0),
        srvhp(0),
        errhp(0),
        usrhp(0),
        svchp(0),
        transactionActive(0)
    {
      std::string conninfo(conninfo_);
      std::string user = extractAttribute(conninfo, "user=");
      std::string passwd = extractAttribute(conninfo, "passwd=");
      logon(conninfo, user, passwd);
    }

    Connection::~Connection()
    {
      if (envhp)
      {
        sword ret;

        clearStatementCache();

        try
        {
          log_debug("OCISessionEnd");
          ret = OCISessionEnd(svchp, errhp, usrhp, OCI_DEFAULT);
          checkError(ret, "OCISessionEnd");
        }
        catch (const std::exception& e)
        {
          log_error(e.what());
        }

        try
        {
          log_debug("OCIServerDetach");
          ret = OCIServerDetach(srvhp, errhp, OCI_DEFAULT);
          checkError(ret, "OCIServerDetach");
        }
        catch (const std::exception& e)
        {
          log_error(e.what());
        }

        log_debug("OCIHandleFree(" << envhp << ')');
        ret = OCIHandleFree(envhp, OCI_HTYPE_ENV);
        if (ret == OCI_SUCCESS)
          log_debug("handle released");
        else
          log_error("OCIHandleFree failed");
      }
    }

    void Connection::beginTransaction()
    {
      //log_debug("OCITransStart(" << svchp << ", " << errhp << ')');
      //checkError(OCITransStart(svchp, errhp, 10, OCI_TRANS_NEW), "OCITransStart");
      ++transactionActive;
    }

    void Connection::commitTransaction()
    {
      if (transactionActive == 0 || --transactionActive == 0)
      {
        log_debug("OCITransCommit(" << srvhp << ", " << errhp << ')');
        checkError(OCITransCommit(svchp, errhp, OCI_DEFAULT), "OCITransCommit");
      }
    }

    void Connection::rollbackTransaction()
    {
      if (transactionActive == 0 || --transactionActive == 0)
      {
        log_debug("OCITransRollback(" << srvhp << ", " << errhp << ')');
        checkError(OCITransRollback(svchp, errhp, OCI_DEFAULT), "OCITransRollback");
      }
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
      return prepare(query).execute();
    }

    tntdb::Result Connection::select(const std::string& query)
    {
      return prepare(query).select();
    }

    Row Connection::selectRow(const std::string& query)
    {
      return prepare(query).selectRow();
    }

    Value Connection::selectValue(const std::string& query)
    {
      return prepare(query).selectValue();
    }

    tntdb::Statement Connection::prepare(const std::string& query)
    {
      return tntdb::Statement(new Statement(this, query));
    }

    void Connection::clearStatementCache()
    {
      IStmtCacheConnection::clearStatementCache();
      seqStmt.clear();
    }

    bool Connection::ping()
    {
      try
      {
        checkError(OCIPing(svchp, errhp, OCI_DEFAULT), "OCIPing");
        return true;
      }
      catch (const Error&)
      {
        return false;
      }
    }

    long Connection::lastInsertId(const std::string& name)
    {
      tntdb::Statement stmt;
      SeqStmtType::iterator s = seqStmt.find(name);
      if (s == seqStmt.end())
      {
        // check for valid sequence name
        for (std::string::const_iterator it = name.begin(); it != name.end(); ++it)
          if (! ((*it >= 'a' && *it <= 'z')
              || (*it >= 'A' && *it <= 'Z')
              || (*it >= '0' && *it <= '9')
              || *it == '_'))
            throw Error("invalid sequence name \"" + name + '"');

        stmt = prepare(
            "select " + name + ".currval from dual");
        seqStmt[name] = stmt;
      }
      else
        stmt = s->second;

      long ret;
      stmt.selectValue().get(ret);
      return ret;
    }

  }
}
