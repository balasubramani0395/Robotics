#include "CPLC_GUI.h"

#include <string>
using namespace Gtk;
#include <stdio.h> 
#include <unistd.h> 
#include <iostream>

CPLC_GUI::CPLC_GUI(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) :
    Gtk::Window(cobject), builder(refGlade){

    builder->get_widget("CONV1", CONV1);
    builder->get_widget("CONV2", CONV2);
    builder->get_widget("CONV3", CONV3);
    builder->get_widget("CONV4", CONV4);
    builder->get_widget("KITCHEN", kitchen);
    builder->get_widget("LINEN", linen);
    builder->get_widget("PHARMA", pharma);
    builder->get_widget("CCSD", CCSD);
    builder->get_widget("TROLLEYREADYSTATUS",  TrolleyReadyButton );
    builder->get_widget("SendData",  SendData );    


    for(int i=0; i<40 ; i++)
    {
        kitchenPLCRegs[i]   = 0;
        linenPLCRegs[i]     = 0;
        pharmaPLCRegs[i]    = 0;
        CCSDPLCRegs[i]      = 0;
    }
    CONV1->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::CONV1Toggled));
    CONV2->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::CONV2Toggled));
    CONV3->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::CONV3Toggled));
    CONV4->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::CONV4Toggled));
    kitchen->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::kitchenToggled));
    linen->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::linenToggled));
    pharma->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::pharmaToggled));
    CCSD->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::CCSDToggled));
    TrolleyReadyButton->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::TrolleyReadyStatusToggled));
    SendData->signal_clicked().connect(sigc::mem_fun(*this, &CPLC_GUI::SendDataToggled));
         
    //create slot for timeout signal
    int timeout_value = 1000; 
    sigc::slot<bool>my_slot = sigc::mem_fun(*this, &CPLC_GUI::update_values);
    //connect slot to signal
    Glib::signal_timeout().connect(my_slot, timeout_value);
    
    kitchenPLC.modbus_connect();
    linenPLC.modbus_connect();
    pharmaPLC.modbus_connect();
    CCSDPLC.modbus_connect();

}
CPLC_GUI::~CPLC_GUI ()
{
    kitchenPLC.modbus_close();
    linenPLC.modbus_close();
    pharmaPLC.modbus_close();
    CCSDPLC.modbus_close();
    delete(&kitchenPLC);
    delete(&linenPLC);
    delete(&pharmaPLC);
    delete(&CCSDPLC);

}

void CPLC_GUI::updateStatusRegs()
{
    if(kitchen->get_active())
    {
        kitchenPLCRegs[39] = 1 ;
        if(CONV1->get_active() && TrolleyReadyButton->get_active()) kitchenPLCRegs[0] = 1 ;
        else kitchenPLCRegs[0] = 0 ;
        if(CONV2->get_active() && TrolleyReadyButton->get_active()) kitchenPLCRegs[10] = 1 ;
        else kitchenPLCRegs[10] = 0 ;
        if(CONV3->get_active() && TrolleyReadyButton->get_active()) kitchenPLCRegs[20] = 1 ;
        else kitchenPLCRegs[20] = 0 ;
        if(CONV4->get_active() && TrolleyReadyButton->get_active()) kitchenPLCRegs[30] = 1 ;
        else kitchenPLCRegs[30] = 0 ; 
    }
    else kitchenPLCRegs[39]  = 0 ;
    if(linen->get_active())
    {
        linenPLCRegs[39] = 1 ;
        if(CONV1->get_active() && TrolleyReadyButton->get_active()) linenPLCRegs[0] = 1 ;
        else linenPLCRegs[0] = 0 ;
        if(CONV2->get_active() && TrolleyReadyButton->get_active()) linenPLCRegs[10] = 1 ;
        else linenPLCRegs[10] = 0 ;
        if(CONV3->get_active() && TrolleyReadyButton->get_active()) linenPLCRegs[20] = 1 ;
        else linenPLCRegs[20] = 0 ;
        if(CONV4->get_active() && TrolleyReadyButton->get_active()) linenPLCRegs[30] = 1 ;
        else linenPLCRegs[30] = 0 ;
    }
    else linenPLCRegs[39]  = 0 ;
    if(pharma->get_active())
    {
        pharmaPLCRegs[39]  = 1 ;
        if(CONV1->get_active() && TrolleyReadyButton->get_active()) pharmaPLCRegs[0] = 1 ;
        else pharmaPLCRegs[0] = 0 ;
        if(CONV2->get_active() && TrolleyReadyButton->get_active()) pharmaPLCRegs[10] = 1 ;
        else pharmaPLCRegs[10] = 0 ;
        if(CONV3->get_active() && TrolleyReadyButton->get_active()) pharmaPLCRegs[20] = 1 ;
        else pharmaPLCRegs[20] = 0 ;
        if(CONV4->get_active() && TrolleyReadyButton->get_active()) pharmaPLCRegs[30] = 1 ;
        else pharmaPLCRegs[30] = 0 ;
    }
    else pharmaPLCRegs[39]  = 0 ;
    if(CCSD->get_active())
    {
        CCSDPLCRegs[39]  = 1 ;
        if(CONV1->get_active() && TrolleyReadyButton->get_active()) CCSDPLCRegs[0] = 1 ;
        else CCSDPLCRegs[0] = 0 ;
        if(CONV2->get_active() && TrolleyReadyButton->get_active()) CCSDPLCRegs[10] = 1 ;
        else CCSDPLCRegs[10] = 0 ;
        if(CONV3->get_active() && TrolleyReadyButton->get_active()) CCSDPLCRegs[20] = 1 ;
        else CCSDPLCRegs[20] = 0 ;
        if(CONV4->get_active() && TrolleyReadyButton->get_active()) CCSDPLCRegs[30] = 1 ;
        else CCSDPLCRegs[30] = 0 ;
    }
    else CCSDPLCRegs[39]  = 0 ;

    
    
}
bool CPLC_GUI::update_values(){
    this->updateStatusRegs();    
    return true;
}

void CPLC_GUI::CONV1Toggled(){ }
void CPLC_GUI::CONV2Toggled(){ }
void CPLC_GUI::CONV3Toggled(){ }
void CPLC_GUI::CONV4Toggled(){ }
void CPLC_GUI::kitchenToggled(){ }
void CPLC_GUI::linenToggled(){ }
void CPLC_GUI::pharmaToggled(){ }
void CPLC_GUI::CCSDToggled(){ }
void CPLC_GUI::TrolleyReadyStatusToggled(){ }
void CPLC_GUI::SendDataToggled(){
    
    pharmaPLC.modbus_write_registers(01, 40, pharmaPLCRegs);
    linenPLC.modbus_write_registers(01, 40, linenPLCRegs);
    CCSDPLC.modbus_write_registers(01, 40, CCSDPLCRegs);
    if(kitchen->get_active()) 
    {
        for(int i=0; i< 40 ; i++)
        {
            cout<< "Index " << i << " Val " << kitchenPLCRegs[i] << endl ;
        }
        
    }
    kitchenPLC.modbus_write_registers(01, 40, kitchenPLCRegs);
    if(linen->get_active()) 
    {
        for(int i=0; i< 40 ; i++)
        {
            cout<< "Index " << i << " Val " << linenPLCRegs[i] << endl ;
        }
        
    }
    linenPLC.modbus_write_registers(01, 40, linenPLCRegs);
    if(pharma->get_active()) 
    {
        for(int i=0; i< 40 ; i++)
        {
            cout<< "Index " << i << " Val " << pharmaPLCRegs[i] << endl ;
        }
    }
        pharmaPLC.modbus_write_registers(01, 40, pharmaPLCRegs);
    if(CCSD->get_active()) 
    {
        for(int i=0; i< 40 ; i++)
        {
            cout<< "Index " << i << " Val " << CCSDPLCRegs[i] << endl ;
        }
    }
    
        CCSDPLC.modbus_write_registers(01, 40, CCSDPLCRegs);
         cout<< "Kitchen " << " Val " << kitchenPLCRegs[39] << endl ;
         cout<< "Linen " <<  " Val " << linenPLCRegs[39] << endl ;
         cout<< "Pharma " << " Val " << pharmaPLCRegs[39] << endl ;
         cout<< "CCSD " <<  " Val " << CCSDPLCRegs[39] << endl ;
  }