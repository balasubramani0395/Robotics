cmake_minimum_required(VERSION 2.8)
project(RTDM)

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

set(MySQLConn /usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so /usr/lib/x86_64-linux-gnu/libmysqlcppconn.so /usr/lib/x86_64-linux-gnu/libmysqlcppconn.so.7 /usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.2 /usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.1 /usr/lib/x86_64-linux-gnu/libmysqlcppconn.so.7.8.0.20 /usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.2.8.0.20 /usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.1.8.0.15)

set(MySQL_INCLUDE_DIRS /usr/include/mysql /usr/include/ /usr/include/mysql-cppconn-8/jdbc/cppconn /usr/include/mysql-cppconn-8)
include_directories(
  include
  ${MySQL_INCLUDE_DIRS}
)

add_executable(Real_Time_Decision_Module
src/RealTimeDecisionModule.cpp
include/RealTimeDecisionModule.h
)

target_link_libraries( Real_Time_Decision_Module  ${MySQLConn} )



