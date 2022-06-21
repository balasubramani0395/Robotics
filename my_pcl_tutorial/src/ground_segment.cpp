#include <ros/ros.h>

#include <iostream>
#include <my_pcl_tutorial/Colour_map.hpp>
#include <my_pcl_tutorial/Color.hpp>
#include <my_pcl_tutorial/planeFitter.hpp>

using namespace std;

int main(int argc,char** argv){

    // Initialize ROS
    ros::init(argc,argv,"planeFitter");
    ros::NodeHandle nh("~");

    planeFitter pf(nh);
    pf.spin();

    return 0;
}