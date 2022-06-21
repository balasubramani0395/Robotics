#include <ros/ros.h>
#include <ros/publisher.h>
#include <sensor_msgs/PointCloud2.h>
#include <dynamic_reconfigure/server.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/PointCloud.h>
// PCL
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/passthrough.h>
#include <pcl/common/transforms.h>
#include <my_pcl_tutorial/Colour_map.hpp>
#include <my_pcl_tutorial/Color.hpp>
#include <my_pcl_tutorial/algorithmParametersConfig.h>
#include <iostream>
using namespace std;
class planeFitter{
public:
    planeFitter(ros::NodeHandle nh);
    void initialize();
    void updateParameters(plane_fitter::algorithmParametersConfig& config, uint32_t level);
    void pointCloudCb(const sensor_msgs::PointCloud2::ConstPtr &msg);
    void createColors();
    void spin();

private:

    // Node
    ros::NodeHandle _nh;
    std::string _name;

    // Publishers
    ros::Publisher _pub_inliers;// Display inliers for each plane

    // Subscriber
    ros::Subscriber _subs;

    // Algorithm parameters
    double _min_percentage;
    double _max_distance;
    bool _color_pc_with_error;

    // Colors
    std::vector<Color> colors;

    // Dynamic reconfigure
    dynamic_reconfigure::Server<plane_fitter::algorithmParametersConfig> dRserver;
    dynamic_reconfigure::Server<plane_fitter::algorithmParametersConfig>::CallbackType drCallback;

};