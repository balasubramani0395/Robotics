#ifndef CONVEYOR_H
#define CONVEYOR_H
#include <string>
#include <iostream>
class Conveyor {
private:
    uint16_t conveyorPLCHealth ;
    uint16_t conveyorPLCStatus ;
    uint16_t conveyorSetDirection ;
    
    uint16_t conveyorGetDirection ;
public:
    // std::pair<string,string> queueUpdate;
    std::string departmentID ;
    std::string conveyorID ;
    uint16_t trolleyReadySignal ;
    uint16_t prevTrolleyReadySignal ;
    uint16_t ConveyorReadRegs[10] ;
    uint16_t ConveyorWriteRegs[10] ;
    Conveyor(std::string deptID, std::string convID);
    void updateStatusValues();
};


#endif //CONVEYOR_H