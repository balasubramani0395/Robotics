
#include "CPLC_GUI.h"
#include <thread>

int main(int argc, char *argv[])
{ 
  Gtk::Main kit(argc, argv);

  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("/home/stk-ait02/Documents/WHC/ProjectModules/ConveyorPLCEmulator/glade/Conveyor PLC Emulator2_4conv.glade");

  CPLC_GUI *frm = 0;
  builder->get_widget_derived("window1", frm);
  kit.run(*frm);
  
  return 0;
}