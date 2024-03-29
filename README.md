Tntdb
=====

Tntdb is a c++-class-library for easy access to databases. The interface is
database-independent. Driverclasses are loaded dynamically.

The goal is to have easy access to databases. You don't have to worry
about resourceleakage, because all objects you use directly are allocated on
the stack and deleted automatically, when going out of scope. The
interfaceobjects are wrappers to referencecounted instances, so you can copy
them around at your own will. Resources are freed, when the last object goes
out of scope.

Errorhandling is done exclusively with exceptionhandling. Just wrap your code
with a single try-catch and you have full errorhandling. All exceptions are
derived from std::exception.

STL is used where possible and also STL-like and -compatible interfaces are
offered, where suitable.

Tntdb is targeted for application programming. It is not suitable for generic
databasetools. It does not offer e.g. schemaquery. You have to know, how your
tables look like. Thats true for normal applications.

You use SQL for databaseaccess. No wrapperclasses like active resultsets or
databound controls are offered. Instead you have best control over the
database. You can use every SQL-trick you know to get most out of your
database. Also this is a thin wrapper, so that you should have no big
performancepenalty compared to direct access to the C-API.

The public headers include documentation, which can be generated using Doxygen.
Also there are some small examples in the demo-directory.

Installation
============

To install tntdb, you need cxxtools (http://www.tntnet.org/).

You can find generic installation instructions in the file INSTALL.

To disable SQLite support (enabled by default) do
./configure --with-sqlite=no

To disable MySQL support (enabled by default) do
./configure --with-mysql=no

To disable Postgresql support (enabled by default) do
./configure --with-postgresql=no

To disable replication driver (enabled by default) do
./configure --with-replicate=no

To enable Oracle-support (disabled by default) do
./configure --with-oracle

HOWTO
=====

A documentation can be found [here](doc/tntdb.markdown)
