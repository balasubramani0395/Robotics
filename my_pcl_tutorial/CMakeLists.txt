cmake_minimum_required(VERSION 2.8.3)
project(my_pcl_tutorial)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

set(OpenCV_INCLUDE_DIRS /home/stk-ait02/opencv-3.4.7/build/include/ /home/stk-ait02/opencv-3.4.7/build/include/opencv2/)
set(OpenCV_DIR /home/stk-ait02/opencv-3.4.7/build)
set(OpenCV_LIBS /home/stk-ait02/opencv-3.4.7/build/lib/libopencv_core.so)
set(CV_BRIDGE_INCLUDE_DIRS /home/stk-ait02/Documents/opencv_test/src/vision_opencv/cv_bridge/include/cv_bridge /home/stk-ait02/Documents/opencv_test/src/vision_opencv/cv_bridge/include/)
set(CV_BRIDGE_DIR /home/stk-ait02/Documents/opencv_test/src/vision_opencv/cv_bridge/include/cv_bridge)

find_package(cv_bridge NO_MODULE  PATHS /home/stk-ait02/Documents/opencv_test/src/vision_opencv cv_bridge/ NO_DEFAULT_PATH )
find_package(OpenCV 3.4.7 REQUIRED 
NO_MODULE  
PATHS /home/stk-ait02/opencv-3.4.7/build/include/ /home/stk-ait02/opencv-3.4.7/build/include/opencv2/ 
NO_DEFAULT_PATH
COMPONENTS
  opencv_core
  opencv_imgproc
  opencv_imgcodecs
  opencv_highgui
  opencv_calib3d
  
CONFIG
)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  pcl_conversions
  pcl_ros
  roscpp
  tf
  sensor_msgs
  laser_geometry
  image_transport
  dynamic_reconfigure
)


catkin_package(

)


include_directories(
include  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${CV_BRIDGE_INCLUDE_DIRS}
)

add_executable(example src/example.cpp )
target_link_libraries(example ${catkin_LIBRARIES} ${OpenCV_LIBS})
add_dependencies(example testin_generate_messages_cpp)


add_executable(ground_segment src/ground_segment.cpp src/Colour_map.cpp src/Color.cpp src/planeFitter.cpp)
target_link_libraries(ground_segment ${catkin_LIBRARIES} ${OpenCV_LIBS})
add_dependencies(ground_segment testin_generate_messages_cpp)

add_executable(laser_cloud src/laser_cloud.cpp)
target_link_libraries(laser_cloud ${catkin_LIBRARIES} ${OpenCV_LIBS})
add_dependencies(laser_cloud testin_generate_messages_cpp)

add_executable(turtle_tf_broadcaster2 src/tf.cpp)
target_link_libraries(turtle_tf_broadcaster2 ${catkin_LIBRARIES})

