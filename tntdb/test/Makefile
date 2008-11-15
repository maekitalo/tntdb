LDFLAGS=-ltntdb
CXXFLAGS=-g
CC=$(CXX)

all: test mysqltest tntdb-mysqltest mysqltest-c

test: test.o

mysqltest: mysqltest.cpp
	$(CXX) -o mysqltest -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient_r mysqltest.cpp

mysqltest-c: mysqltest-c.c
	gcc -o mysqltest-c -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient_r mysqltest-c.c

tntdb-mysqltest: tntdb-mysqltest.cpp
	$(CXX) -o tntdb-mysqltest -ltntdb tntdb-mysqltest.cpp
