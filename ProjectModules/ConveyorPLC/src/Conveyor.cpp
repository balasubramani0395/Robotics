#include "Conveyor.h"


Conveyor::Conveyor(std::string deptID, std::string convID)
{
    departmentID       =   deptID  ;
    conveyorID         =   convID  ;

}

void  Conveyor::updateStatusValues()
{
    trolleyReadySignal = ConveyorReadRegs[0];
    std::cout << " TR " << trolleyReadySignal << std::endl ;
}

