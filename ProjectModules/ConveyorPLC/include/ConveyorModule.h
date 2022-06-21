
#ifndef CONVEYORMODULE_H
#define CONVEYORMODULE_H

#include "Conveyor.h"
#include "modbus.h"
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
#define DATABASE "OPSVMdb"

std::queue<std::pair<string,string>> SQLdbUpdateQueue;


// Conveyor Department 
modbus kitchenPLC("127.0.0.1", 2000);
uint16_t kitchenPLCReadRegs[40];
uint16_t kitchenPLCWriteRegs[40];

Conveyor kitchen1 = Conveyor("kitchen", "kitchen1" );
Conveyor kitchen2 = Conveyor("kitchen", "kitchen2" );
Conveyor kitchen3 = Conveyor("kitchen", "kitchen3" );
Conveyor kitchen4 = Conveyor("kitchen", "kitchen4" );
void kitchenPLCRegsUpdate();

// Linen Department 
modbus linenPLC("127.0.0.1", 2001);
uint16_t linenPLCReadRegs[40];
uint16_t linenPLCWriteRegs[40];
Conveyor linen1 = Conveyor("linen", "linen1" );
Conveyor linen2 = Conveyor("linen", "linen2" );
Conveyor linen3 = Conveyor("linen", "linen3" );
Conveyor linen4 = Conveyor("linen", "linen4" );
void linenPLCRegsUpdate();


// Pharma Department 
modbus pharmaPLC("127.0.0.1", 2002);
uint16_t pharmaPLCReadRegs[40];
uint16_t pharmaPLCWriteRegs[40];
Conveyor pharma1 = Conveyor("pharma", "pharma1" );
Conveyor pharma2 = Conveyor("pharma", "pharma2" );
Conveyor pharma3 = Conveyor("pharma", "pharma3" );
Conveyor pharma4 = Conveyor("pharma", "pharma4" );
void pharmaPLCRegsUpdate();

// CCSD Department 
modbus CCSDPLC("127.0.0.1", 2003);
uint16_t CCSDPLCReadRegs[40];
uint16_t CCSDPLCWriteRegs[40];
Conveyor CCSD1 = Conveyor("CCSD", "CCSD1" );
Conveyor CCSD2 = Conveyor("CCSD", "CCSD2" );
Conveyor CCSD3 = Conveyor("CCSD", "CCSD3" );
Conveyor CCSD4 = Conveyor("CCSD", "CCSD4" );
void CCSDPLCRegsUpdate();

void updateScheduleJobDB();

#endif