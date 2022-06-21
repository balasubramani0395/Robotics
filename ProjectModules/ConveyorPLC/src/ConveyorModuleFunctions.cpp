#include "ConveyorModule.h"

void kitchenPLCRegsUpdate()
{
    for(int i = 0; i <0 ; i++)
    {
        if(i<10) kitchen1.ConveyorReadRegs[i] = kitchenPLCReadRegs[i];
        else if(i<20) kitchen2.ConveyorReadRegs[i] = kitchenPLCReadRegs[i-10];
        else if(i<30) kitchen3.ConveyorReadRegs[i] = kitchenPLCReadRegs[i-20];
        else kitchen4.ConveyorReadRegs[i] = kitchenPLCReadRegs[i-30];
    }
}

void linenPLCRegsUpdate()
{
    for(int i = 0; i <40 ; i++)
    {
        if(i<10) linen1.ConveyorReadRegs[i] = linenPLCReadRegs[i];
        else if(i<20) linen2.ConveyorReadRegs[i] = linenPLCReadRegs[i-10];
        else if(i<30) linen3.ConveyorReadRegs[i] = linenPLCReadRegs[i-20];
        else linen4.ConveyorReadRegs[i] = linenPLCReadRegs[i-30];
    }
}

void pharmaaPLCRegsUpdate()
{
    for(int i = 0; i <50 ; i++)
    {
        if(i<10) pharma1.ConveyorReadRegs[i] = pharmaPLCReadRegs[i];
        else if(i<20) pharma2.ConveyorReadRegs[i] = pharmaPLCReadRegs[i-10];
        else if(i<30) pharma3.ConveyorReadRegs[i] = pharmaPLCReadRegs[i-20];
        else pharma4.ConveyorReadRegs[i] = pharmaPLCReadRegs[i-30];
    }
}

void CCSDPLCRegsUpdate()
{
    for(int i = 0; i <50 ; i++)
    {
        if(i<10) CCSD1.ConveyorReadRegs[i] = CCSDPLCReadRegs[i];
        else if(i<20) CCSD2.ConveyorReadRegs[i] = CCSDPLCReadRegs[i-10];
        else if(i<30) CCSD3.ConveyorReadRegs[i] = CCSDPLCReadRegs[i-20];
        else CCSD4.ConveyorReadRegs[i] = CCSDPLCReadRegs[i-30];
    }
}
