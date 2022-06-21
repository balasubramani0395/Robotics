cmake_minimum_required(VERSION 2.8.3)
project(rack_detection)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  pcl_conversions
  pcl_ros
  roscpp
  tf
  sensor_msgs
  laser_geometry
  dynamic_reconfigure
)
catkin_package(
)

include_directories(
include  ${catkin_INCLUDE_DIRS}
)

add_executable(rack src/newTutorial.cpp)
target_link_libraries(rack ${catkin_LIBRARIES} ${OpenCV_LIBS})
add_dependencies(rack testin_generate_messages_cpp)