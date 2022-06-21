#include <gtkmm.h>
#include "modbus.h"
#include <iostream>

class CPLC_GUI : public Gtk::Window{
protected:
    Glib::RefPtr<Gtk::Builder> builder;

    int recStop ;
    int dataToggle=0;
    std::string sendTextStr;
    Gtk::Entry *sendText;
    Gtk::TextView *recText;
    Gtk::Label *conveyorDirection ;

    uint16_t acknowledge ;
    modbus kitchenPLC = modbus("127.0.0.1", 2000);
    modbus linenPLC = modbus("127.0.0.1", 2001);
    modbus pharmaPLC = modbus("127.0.0.1", 2002);
    modbus CCSDPLC = modbus("127.0.0.1", 2003);

    uint16_t statusSendRegs[40];
    
    uint16_t kitchenPLCRegs[40] , linenPLCRegs[40] , pharmaPLCRegs[40], CCSDPLCRegs[40];


    void updateStatusRegs();

public:
    CPLC_GUI(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);//constructor
    ~CPLC_GUI ();
    bool update_values();
    int start;
protected:


    //PLC Selection
    void CONV1Toggled();
    void CONV2Toggled();
    void CONV3Toggled();
    void CONV4Toggled();
    void kitchenToggled();
    void linenToggled();
    void pharmaToggled();
    void CCSDToggled();
    void TrolleyReadyStatusToggled();
    void SendDataToggled();
    Gtk::ToggleButton *CONV1;
    Gtk::ToggleButton *CONV2;
    Gtk::ToggleButton *CONV3;
    Gtk::ToggleButton *CONV4;
    Gtk::ToggleButton *kitchen;
    Gtk::ToggleButton *linen;
    Gtk::ToggleButton *pharma;
    Gtk::ToggleButton *CCSD;
    Gtk::ToggleButton *TrolleyReadyButton;
    Gtk::Button *SendData;

       
};