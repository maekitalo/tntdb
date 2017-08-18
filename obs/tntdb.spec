#
# spec file for package tntdb
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# Note: for 42ity, maybe this should require (or optionalize) mariadb-devel
# if defined on target distro at all

Name:           tntdb
Version:        1.3
Release:        1
Summary:        Library for simple database access
License:        GPL-2.0+
Group:          Development/Libraries/C and C++
Url:            http://www.tntnet.org/index.html
Source0:        tntdb-%{version}.tar.gz
Patch0:         tntdb-1.3-avoid-version.patch
#Patch1:         connection.patch
#Patch2:         0002-add-pkgconfig-support.patch
BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  findutils
BuildRequires:  gcc-c++
BuildRequires:  glibc-devel
BuildRequires:  gnutls-devel
BuildRequires:  libcxxtools-devel
BuildRequires:  libtool
BuildRequires:  lzo
BuildRequires:  lzo-devel
BuildRequires:  mysql-devel
BuildRequires:  openssl-devel
BuildRequires:  pkg-config
BuildRequires:  postgresql-devel
BuildRequires:  sqlite-devel
BuildRequires:  zip
BuildRequires:  zlib-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Tntdb is a library for simple database access.

The database independent layer offers easy to use methods for working with the database and also greatly simplifies resource-management. The classes hold reference-counted pointers to the actual implementation. They are copyable and assignable. The user can use the classes just like simple values. The resources they reference are freed, when the last reference is deleted. This happens normally just by leaving the scope. There is normally no reason to instantiate them dynamically on the heap.

The driver-layer contains the actual implementation, which does the work. These classes are database-dependent. The user normally doesn't need to deal with this.

%package -n libtntdb4
Summary:        Library for simple database access
Group:          Development/Libraries/C and C++
Provides:       tntdb

%description -n libtntdb4
Tntdb is a library for simple database access.

The database independent layer offers easy to use methods for working with the database and also greatly simplifies resource-management. The classes hold reference-counted pointers to the actual implementation. They are copyable and assignable. The user can use the classes just like simple values. The resources they reference are freed, when the last reference is deleted. This happens normally just by leaving the scope. There is normally no reason to instantiate them dynamically on the heap.

The driver-layer contains the actual implementation, which does the work. These classes are database-dependent. The user normally doesn't need to deal with this.

%package -n libtntdb-devel
Summary:        Development files for tntdb
Group:          Development/Libraries/C and C++
Provides:       tntdb-devel
Requires:       cxxtools-devel
Requires:       libtntdb4 = %{version}

%description -n libtntdb-devel
Headers and so links for tntdb library.

%package -n tntdb-mysql
Summary:        MySQL plugin for tntdb
Group:          Development/Libraries/C and C++

%description -n tntdb-mysql
MySQL plugin for tntdb abstraction library.

%package -n tntdb-postgresql
Summary:        PostgreSQL plugin for tntdb
Group:          Development/Libraries/C and C++

%description -n tntdb-postgresql
PostgreSQL plugin for tntdb abstraction library.

%package -n tntdb-sqlite
Summary:        SQLite plugin for tntdb
Group:          Development/Libraries/C and C++

%description -n tntdb-sqlite
SQLite plugin for tntdb abstraction library.

%package -n tntdb-replicate
Summary:        SQLite plugin for tntdb
Group:          Development/Libraries/C and C++

%description -n tntdb-replicate
Replication plugin for tntdb abstraction library.

%prep
%setup -q -n tntdb-%{version}
if [ "`find tntdb/ | wc -l`" -gt 1 ] ; then mv -f tntdb/* ./ ; fi
%patch0
#%patch1 -p1
#%patch2 -p1

%build
autoreconf -vi
%configure --with-doxygen=no
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install %{?_smp_mflags}
find %{buildroot} -type f -name "*.la" -delete -print
find %{buildroot} -name '*.a' -delete

%post -n libtntdb4 -p /sbin/ldconfig

%postun -n libtntdb4 -p /sbin/ldconfig

%files -n libtntdb4
%defattr (-, root, root)
%{_libdir}/libtntdb.so.*

%files -n libtntdb-devel
%defattr (-, root, root)
%doc AUTHORS ChangeLog README
%{_includedir}/tntdb
%{_includedir}/tntdb.h
%{_libdir}/libtntdb.so
%{_libdir}/pkgconfig/*.pc

%files -n tntdb-mysql
%defattr (-, root, root)
%dir %{_libdir}/tntdb
%{_libdir}/tntdb/tntdb*mysql.so

%files -n tntdb-postgresql
%defattr (-, root, root)
%dir %{_libdir}/tntdb
%{_libdir}/tntdb/tntdb*postgresql.so

%files -n tntdb-sqlite
%defattr (-, root, root)
%dir %{_libdir}/tntdb
%{_libdir}/tntdb/tntdb*sqlite.so

%files -n tntdb-replicate
%defattr (-, root, root)
%dir %{_libdir}/tntdb
%{_libdir}/tntdb/tntdb*replicate.so

%changelog
