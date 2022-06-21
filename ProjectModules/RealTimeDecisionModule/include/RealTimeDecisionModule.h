#ifndef RTDMMODULE_H
#define RTDMMODULE_H

#include <iostream>



#include <iostream>
#include <string>
#include <queue>          // std::queue


/* MySQL Connector/C++ specific headers */

#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <metadata.h>
#include <resultset_metadata.h>
#include <exception.h>
#include <warning.h>

using namespace sql;
using namespace std;

Driver *driver;
Connection *con;
Statement *stmt;
ResultSet *res;
PreparedStatement *prep_stmt;
Savepoint *savept;

int updatecount = 0;

#define DBHOST "tcp://127.0.0.1:3306"
#define USER "root"
#define PASSWORD "root"
#define DATABASE "OPSVMDB"

std::queue<std::pair<string,string>> SQLdbUpdateQueue;


#endif