#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <turtlesim/Pose.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"
std::string turtle_name;


ros::Publisher orbPub ;
geometry_msgs::PoseWithCovarianceStamped amclPose ;
void navCallback(const nav_msgs::Odometry::Ptr& msg){
  static tf::TransformBroadcaster br;
  nav_msgs::Odometry navPose ;
  tf::Transform transform;
  navPose = *msg ;
  transform.setOrigin( tf::Vector3(navPose.pose.pose.position.x, navPose.pose.pose.position.y, 0.0) );
  tf::Quaternion q;
  q[2] = navPose.pose.pose.orientation.z - 0.055;
  q[3] = navPose.pose.pose.orientation.w ;
  q[0] = 0 ;
  q[1] = 0 ;
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "map", "laser"));

}

void poseCallback(const geometry_msgs::PoseWithCovarianceStamped::Ptr& msg){

  amclPose = *msg ;


}

void poseCallback2(const geometry_msgs::PoseStamped::Ptr& msg)
{
    geometry_msgs::PoseWithCovarianceStamped orbPose;

    orbPose.pose.pose.position.x = msg->pose.position.x ;
    orbPose.pose.pose.position.y = msg->pose.position.y ;
    orbPose.pose.pose.orientation.z = msg->pose.orientation.z ;
    orbPose.pose.pose.orientation.w = msg->pose.orientation.w ;
    orbPose.pose.covariance = amclPose.pose.covariance ;
    orbPub.publish(orbPose);
}

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_broadcaster");


  ros::NodeHandle node;

  ros::Subscriber sub = node.subscribe("/odometry/filtered", 10, &navCallback);
  ros::Subscriber sub3 = node.subscribe("/amcl_pose", 10, &poseCallback);
  ros::Subscriber sub2 = node.subscribe("/ORB_SLAM/pose", 10, &poseCallback2);
  orbPub = node.advertise<geometry_msgs::PoseWithCovarianceStamped>("/ORB_SLAM/poseCov", 1);
  
  ros::spin();
  return 0;
};
