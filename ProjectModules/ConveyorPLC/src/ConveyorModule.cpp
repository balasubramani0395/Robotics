#include "ConveyorModule.h"

void checkSignalUpdate(Conveyor conv)
{
    cout << conv.conveyorID  << " tR3 " << conv.trolleyReadySignal << " Ptr3  " << conv.prevTrolleyReadySignal  <<  endl;
    if(conv.trolleyReadySignal != conv.prevTrolleyReadySignal )  
    {  
        cout << "tR1 " << conv.trolleyReadySignal << "Ptr1 " << conv.prevTrolleyReadySignal  <<  endl;
        if(conv.trolleyReadySignal)
        {
        SQLdbUpdateQueue.push(pair<string,string>(conv.departmentID, conv.conveyorID));
        cout << "tR2 " << conv.trolleyReadySignal << "Ptr2 " << conv.prevTrolleyReadySignal  <<  endl;
        }
    }
}

void kitchenPLCRegsUpdate()
{
    for(int i = 0; i <40 ; i++)
    {
        if(i<10) kitchen1.ConveyorReadRegs[i] = kitchenPLCReadRegs[i];
        else if(i<20) kitchen2.ConveyorReadRegs[i-10] = kitchenPLCReadRegs[i]; 
        else if(i<30) kitchen3.ConveyorReadRegs[i-20] = kitchenPLCReadRegs[i];
        else kitchen4.ConveyorReadRegs[i-30] = kitchenPLCReadRegs[i];
        cout<< "Index " << i << " Val " << kitchenPLCReadRegs[i] << endl ;
    }
    kitchen1.updateStatusValues();
    kitchen2.updateStatusValues();
    kitchen3.updateStatusValues();
    kitchen4.updateStatusValues();
    checkSignalUpdate(kitchen1);
    kitchen1.prevTrolleyReadySignal = kitchen1.trolleyReadySignal;
    checkSignalUpdate(kitchen2);
    kitchen2.prevTrolleyReadySignal = kitchen2.trolleyReadySignal;
    checkSignalUpdate(kitchen3);
    kitchen3.prevTrolleyReadySignal = kitchen3.trolleyReadySignal;
    checkSignalUpdate(kitchen4);
    kitchen4.prevTrolleyReadySignal = kitchen4.trolleyReadySignal;
    cout << "Kitchen block running " << endl;
 
}

void linenPLCRegsUpdate()
{
    for(int i = 0; i <40 ; i++)
    {
        if(i<10) linen1.ConveyorReadRegs[i] = linenPLCReadRegs[i];
        else if(i<20) linen2.ConveyorReadRegs[i-10] = linenPLCReadRegs[i];
        else if(i<30) linen3.ConveyorReadRegs[i-20] = linenPLCReadRegs[i];
        else linen4.ConveyorReadRegs[i-30] = linenPLCReadRegs[i];
        cout<< "Index " << i << " Val " << linenPLCReadRegs[i] << endl ;
    }
    linen1.updateStatusValues();
    linen2.updateStatusValues();
    linen3.updateStatusValues();
    linen4.updateStatusValues();
    checkSignalUpdate(linen1);
    linen1.prevTrolleyReadySignal = linen1.trolleyReadySignal;
    checkSignalUpdate(linen2);
    linen2.prevTrolleyReadySignal = linen2.trolleyReadySignal;
    checkSignalUpdate(linen3);
    linen3.prevTrolleyReadySignal = linen3.trolleyReadySignal;
    checkSignalUpdate(linen4);
    linen4.prevTrolleyReadySignal = linen4.trolleyReadySignal;

    cout << "linen block running " << endl;

}

void pharmaPLCRegsUpdate()
{
    for(int i = 0; i <40 ; i++)
    {
        if(i<10) pharma1.ConveyorReadRegs[i] = pharmaPLCReadRegs[i];
        else if(i<20) pharma2.ConveyorReadRegs[i-10] = pharmaPLCReadRegs[i];
        else if(i<30) pharma3.ConveyorReadRegs[i-20] = pharmaPLCReadRegs[i];
        else pharma4.ConveyorReadRegs[i-30] = pharmaPLCReadRegs[i];
        cout<< "Index " << i << " Val " << pharmaPLCReadRegs[i] << endl ;
    }
    pharma1.updateStatusValues();
    pharma2.updateStatusValues();
    pharma3.updateStatusValues();
    pharma4.updateStatusValues();
    checkSignalUpdate(pharma1);
    pharma1.prevTrolleyReadySignal = pharma1.trolleyReadySignal;
    checkSignalUpdate(pharma2);
    pharma2.prevTrolleyReadySignal = pharma2.trolleyReadySignal;
    checkSignalUpdate(pharma3);
    pharma3.prevTrolleyReadySignal = pharma3.trolleyReadySignal;
    checkSignalUpdate(pharma4);
    pharma4.prevTrolleyReadySignal = pharma4.trolleyReadySignal;
    cout << "pharma block running " << endl;

}

void CCSDPLCRegsUpdate()
{
    for(int i = 0; i <40 ; i++)
    {
        if(i<10) CCSD1.ConveyorReadRegs[i] = CCSDPLCReadRegs[i];
        else if(i<20) CCSD2.ConveyorReadRegs[i-10] = CCSDPLCReadRegs[i];
        else if(i<30) CCSD3.ConveyorReadRegs[i-20] = CCSDPLCReadRegs[i];
        else CCSD4.ConveyorReadRegs[i-30] = CCSDPLCReadRegs[i];
        cout<< "Index " << i << " Val " << CCSDPLCReadRegs[i] << endl ;
    }
    CCSD1.updateStatusValues();
    CCSD2.updateStatusValues();
    CCSD3.updateStatusValues();
    CCSD4.updateStatusValues();
    checkSignalUpdate(CCSD1);
    CCSD1.prevTrolleyReadySignal = CCSD1.trolleyReadySignal;
    checkSignalUpdate(CCSD2);
    CCSD2.prevTrolleyReadySignal = CCSD2.trolleyReadySignal;
    checkSignalUpdate(CCSD3);
    CCSD3.prevTrolleyReadySignal = CCSD3.trolleyReadySignal;
    checkSignalUpdate(CCSD4);
    CCSD4.prevTrolleyReadySignal = CCSD4.trolleyReadySignal;
    cout << "CCSD block running " << endl;

}
void updateScheduleJobDB()
{
    std::pair<string,string> dbUpdateTemp ;
    while(!SQLdbUpdateQueue.empty())
    {
        dbUpdateTemp = SQLdbUpdateQueue.front() ;
        SQLdbUpdateQueue.pop();
        cout << "departmentID " << dbUpdateTemp.first << " conveyorID " << dbUpdateTemp.second  <<  endl;

        try
        {
            cout << "Inserting AGV Job Data .. " << endl << endl;

            //	insert couple of rows of data into City table using Prepared Statements 
            prep_stmt = con -> prepareStatement ("INSERT INTO ScheduleJobCreation (DateID,DeptID,ConvID) VALUES (?,?,?)");

            prep_stmt -> setString (1, "17052020" );
            prep_stmt -> setString (2, dbUpdateTemp.first );
            prep_stmt -> setString (3, dbUpdateTemp.second );
            updatecount = prep_stmt -> executeUpdate();

            cout << "\tCommitting outstanding updates to the database .." << endl;
            con -> commit();
        } catch (SQLException &e) {
                cout << "ERROR: SQLException in " << __FILE__;
                cout << " (" << __func__<< ") on line " << __LINE__ << endl;
                cout << "ERROR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << ")" << endl;

                if (e.getErrorCode() == 1047) {
                    /*
                    Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
                    Message: Unknown command
                    */
                    cout << "\nYour server does not seem to support Prepared Statements at all. ";
                    cout << "Perhaps MYSQL < 4.1?" << endl;
                }

            // return EXIT_FAILURE;
        } catch (std::runtime_error &e) {

            cout << "ERROR: runtime_error in " << __FILE__;
            cout << " (" << __func__ << ") on line " << __LINE__ << endl;
            cout << "ERROR: " << e.what() << endl;

            // return EXIT_FAILURE;
        }

	    // return EXIT_SUCCESS;
    }

}
int main(int argc, char *argv[])
{
    kitchenPLC.modbus_connect();
    linenPLC.modbus_connect();
    pharmaPLC.modbus_connect();
    CCSDPLC.modbus_connect();
    

    //OPSVMDB.ScheduleJobCreation
    

    /* initiate url, user, password and database variables */
    string url(argc >= 2 ? argv[1] : DBHOST);
    const string user(argc >= 3 ? argv[2] : USER);
    const string password(argc >= 4 ? argv[3] : PASSWORD);
    const string database(argc >= 5 ? argv[4] : DATABASE);
    driver = get_driver_instance();
		
    /* create a database connection using the Driver */
    con = driver -> connect(url, user, password);

    /* alternate syntax using auto_ptr to create the db connection */
    //auto_ptr  con (driver -> connect(url, user, password));

    /* turn off the autocommit */
    con -> setAutoCommit(0);

    cout << "\nDatabase connection\'s autocommit mode = " << con -> getAutoCommit() << endl;

    /* select appropriate database schema */
    con -> setSchema(database);

    /* retrieve and display the database metadata */
    // retrieve_dbmetadata_and_print (con);

    /* create a statement object */
    stmt = con -> createStatement();
    while(true)
    {
        // Kitchen Conveyor Operations
        kitchenPLC.modbus_read_holding_registers(01, 40, kitchenPLCReadRegs);
        if(kitchenPLCReadRegs[39]) kitchenPLCRegsUpdate();

        // sleep(1);
        // Linen Conveyor Operations
        linenPLC.modbus_read_holding_registers(01, 40, linenPLCReadRegs);
        if(linenPLCReadRegs[39]) linenPLCRegsUpdate();
        // sleep(1);
        // Pharma Conveyor Operations
        pharmaPLC.modbus_read_holding_registers(01, 40, pharmaPLCReadRegs);
        if(pharmaPLCReadRegs[39]) pharmaPLCRegsUpdate();
        sleep(1);
        // CCSD Conveyor Operations
        CCSDPLC.modbus_read_holding_registers(01, 40, CCSDPLCReadRegs);
        if(CCSDPLCReadRegs[39]) CCSDPLCRegsUpdate();
        cout<< "Kitchen " << " Val " << kitchenPLCReadRegs[39] << endl ;
         cout<< "Linen " <<  " Val " << linenPLCReadRegs[39] << endl ;
         cout<< "Pharma " << " Val " << pharmaPLCReadRegs[39] << endl ;
         cout<< "CCSD " <<  " Val " << CCSDPLCReadRegs[39] << endl ;
        if(!SQLdbUpdateQueue.empty())   updateScheduleJobDB();
    }
    kitchenPLC.modbus_close();
    linenPLC.modbus_close();
    pharmaPLC.modbus_close();
    CCSDPLC.modbus_close();
    delete(&kitchenPLC);
    delete(&linenPLC);
    delete(&pharmaPLC);
    delete(&CCSDPLC);

    delete res;
    delete stmt;
    delete prep_stmt;
    con -> close();
    delete con;
}