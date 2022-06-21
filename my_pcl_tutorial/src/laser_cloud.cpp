#include <my_pcl_tutorial/laser_cloud.hpp>

//comparator functions
bool less_by_x(const cv::Point2f& lhs, const cv::Point2f& rhs)
{
  return lhs.x<rhs.x;
}

bool less_by_y(const cv::Point2f& lhs, const cv::Point2f& rhs)
{
  return lhs.y<rhs.y;
}

void scan_cb (const sensor_msgs::LaserScan::ConstPtr& input)
{
  float reading;
  laser_geometry::LaserProjection projector_;
  sensor_msgs::PointCloud2 cloud;
  sensor_msgs::PointCloud out_pointcloud;
  
  //convert PCL2 to PCL
  projector_.projectLaser(*input, cloud);
  sensor_msgs::convertPointCloud2ToPointCloud(cloud, out_pointcloud);
  cloud.header.frame_id="cloud2";
  reading=ceil((input->angle_max-input->angle_min)/input->angle_increment);
  cloud.header.stamp=ros::Time::now();
  Point2f point;
  vector<Point2f> image_points;
  Point2f min;
  Point2f max;
  int width;
  int height;
  int x_img;
  int y_img;
  int resolution;
  for(int i = 0 ; i < out_pointcloud.points.size(); ++i){
    point.x = out_pointcloud.points[i].x;
    point.y = out_pointcloud.points[i].y;
    image_points.push_back(point);

  }
  auto mmx_x = minmax_element(image_points.begin(),image_points.end(),less_by_x);
  auto mmx_y = minmax_element(image_points.begin(),image_points.end(),less_by_y);

  
  min.x =mmx_x.first->x;
  min.y =mmx_y.first->y;
  max.x =mmx_x.second->x;
  max.y =mmx_y.second->y;
  cout<<min.x<<","<<min.y<<endl;
  cout<<max.x<<","<<max.y<<endl;
  resolution=5;
  width = (abs(min.x)+max.x)*resolution;
  height = (abs(min.y)+max.y)*resolution;
  cout<<width<<","<<height<<endl;
  Mat processed(Size(1000,1000),CV_8UC1,Scalar(0));
   
  //convert into image
  for(int i = 0 ; i < image_points.size(); ++i){
    x_img=int((image_points[i].x+abs(min.x))*resolution);
    y_img=int((image_points[i].y+abs(min.y))*resolution);
    if(x_img<width && x_img>=0 && y_img<height && y_img>=0)
    {
      cv::line(processed, Point(abs(min.y*resolution),abs(min.x*resolution)), Point(y_img,x_img), cv::Scalar(255), 1);
    }
    else if(x_img<0 && y_img<0)
    {
      cout<<x_img<<","<<y_img<<endl;
        cout<<"negative"<<endl;
    }
    else
    {
      cout<<x_img<<","<<y_img<<endl;
      cout<<"out of range"<<endl;
    }
    
  }
  cv_bridge::CvImagePtr cv_ptr;
  cv_bridge::CvImage out_msg;
  
  out_msg.encoding = sensor_msgs::image_encodings::TYPE_8UC1;
  out_msg.image =processed;
  //show image
  imshow("dist",processed);
  waitKey(3);
  //publish cloud
  pub.publish(cloud);

}


int main (int argc, char** argv)
{
  // Initialize ROS
  
  ros::init (argc, argv, "laser_cloud");
  ros::NodeHandle nh;
  
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe ("scan", 1, scan_cb);
  
  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("scan_cloud", 1);
  //image_pub = nh.advertise<sensor_msgs::Image> ("map", 1);


  // Spin
  ros::spin ();
}
