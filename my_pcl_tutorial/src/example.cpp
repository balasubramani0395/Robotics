#include <my_pcl_tutorial/example.hpp>

void cloud_cb (const sensor_msgs::PointCloud2ConstPtr& input)
{
  // Create a container for the data.
  sensor_msgs::PointCloud2 output;
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_msg (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::fromROSMsg(*input,*cloud_msg);

  // Create the filtering object
  pcl::PassThrough<pcl::PointXYZ> pass;
  pass.setInputCloud(cloud_msg);
  pass.setFilterFieldName ("z");
  pass.setFilterLimits(-2.5,-1.5);
  pass.setFilterLimits(-1.5,2);
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_f (new pcl::PointCloud<pcl::PointXYZ>);
  pass.filter (*cloud);
  
  *cloud_f=*cloud;

  pcl::VoxelGrid<pcl::PointXYZ> sor;
  sor.setInputCloud(cloud_f);
  sor.setLeafSize (0.01f, 0.01f, 100000.0f);
  sor.filter (*cloud_filtered);
  
  // Publish points
  sensor_msgs::PointCloud2 cloud_publish;
  
  pcl::toROSMsg(*cloud_f,output);

  output.header.frame_id = "segment";

  pub.publish (output);
}


int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "pcl_filter");
  ros::NodeHandle nh;
  
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe ("velodyne_points", 1, cloud_cb);
  
  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("output", 1);
  

  // Spin
  ros::spin ();
}
