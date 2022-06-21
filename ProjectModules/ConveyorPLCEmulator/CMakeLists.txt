cmake_minimum_required(VERSION 2.8)
project(COnveyorplc)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)


set(Project_Include_DIRS /include/ )

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 include
  ${Project_Include_DIRS}

)

## Declare a C++ library
# add_library(${PROJECT_NAME}
#   src/${PROJECT_NAME}/pcl_intensity_filter.cpp
# )

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/pcl_intensity_filter_node.cpp)

#FIND_PACKAGE ( PkgConfig REQUIRED )
#PKG_CHECK_MODULES( GTK REQUIRED gtk+-3.0 ) # or use gtk+-2.0, etc...
INCLUDE_DIRECTORIES ( ${GTK_INCLUDE_DIRS} )


FIND_PACKAGE ( PkgConfig REQUIRED )
PKG_CHECK_MODULES( GTKMM REQUIRED gtkmm-3.0 ) # or use gtk+-2.0, etc...
INCLUDE_DIRECTORIES ( ${GTKMM_INCLUDE_DIRS} )
SET ( FOOBAR_LIBS ${GTKMM_LIBRARIES}  ${GTK_LIBRARIES})

add_executable(Conveyor_PLC_Hardware_Emulator
src/CPLC_EMU_Main.cpp
src/CPLC_GUI.cpp
src/CPLC_GUI.h
src/modbus.cpp
include/modbus.h)
target_link_libraries( Conveyor_PLC_Hardware_Emulator ${FOOBAR_LIBS} pthread)

#add_executable(Conveyor_PLC_Interface
#src/OLD/OPSVM0001.cpp
#src/OLD/OPSVM0001.h
#src/modbus.cpp
#include/modbus.h)

#target_link_libraries( Conveyor_PLC_Interface boost_system ${FOOBAR_LIBS} pthread jsoncpp)



## Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
# )




