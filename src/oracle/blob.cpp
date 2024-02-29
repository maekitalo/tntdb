/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#include <tntdb/oracle/blob.h>
#include <cxxtools/log.h>
#include <string.h>

log_define("tntdb.oracle.blob")

namespace tntdb
{
namespace oracle
{
// oci-wrappers

void Blob::ociDescriptorAlloc()
{
    log_debug("OCIDescriptorAlloc(OCI_DTYPE_LOB)");
    sword ret = OCIDescriptorAlloc(_conn->getEnvHandle(),
        reinterpret_cast<void**>(&_lob), OCI_DTYPE_LOB, 0, 0);
    _conn->checkError(ret, "OCIDescriptorAlloc(OCI_DTYPE_LOB)");
}

void Blob::ociDescriptorFree()
{
    log_debug("OCIDescriptorFree(" << _lob << ", OCI_DTYPE_LOB)");
    OCIDescriptorFree(_lob, OCI_DTYPE_LOB);
}

// ctors, dtors, ...
//
Blob::Blob(Connection& conn, OCILobLocator* lob, bool release)
  : _conn(&conn),
    _lob(lob),
    _release(release)
{ }

Blob::Blob(Connection& conn, const char* data, ub4 count)
  : _conn(&conn), _lob(0), _release(true)
{
    log_debug("create oracle::Blob from data; size=" << count);

    ociDescriptorAlloc();

    log_debug("OCILobWrite");
    sword ret = OCILobWrite(_conn->getSvcCtxHandle(), _conn->getErrorHandle(),
        _lob, &count, 0, const_cast<char*>(data), count, OCI_ONE_PIECE,
        0, 0, 0, 0);
    _conn->checkError(ret, "OCILobWrite");
}

void Blob::getData(tntdb::Blob& blob) const
{
    log_debug("OCILobGetLength");
    ub4 len;
    sword ret = OCILobGetLength(_conn->getSvcCtxHandle(), _conn->getErrorHandle(),
        _lob, &len);
    _conn->checkError(ret, "OCILobGetLength");
    log_debug("len=" << len);

    log_debug("OCILobOpen");
    ret = OCILobOpen(_conn->getSvcCtxHandle(), _conn->getErrorHandle(),
        _lob, OCI_LOB_READONLY);
    _conn->checkError(ret, "OCILobOpen");

    char* buffer = blob.reserve(len, true);
    memset(buffer, '\0', len);

    ub4 amt = len;
    log_debug("OCILobRead");
    ret = OCILobRead(_conn->getSvcCtxHandle(), _conn->getErrorHandle(),
        _lob, &amt, 1, buffer, len, 0, 0, 0, SQLCS_IMPLICIT);
    _conn->checkError(ret, "OCILobRead");

    log_debug("OCILobClose");
    ret = OCILobClose(_conn->getSvcCtxHandle(), _conn->getErrorHandle(),
        _lob);
    _conn->checkError(ret, "OCILobClose");
}

OCILobLocator*& Blob::getHandle(Connection* conn)
{
    _conn = conn;
    if (_lob == 0)
        ociDescriptorAlloc();

    return _lob;
}

}
}
