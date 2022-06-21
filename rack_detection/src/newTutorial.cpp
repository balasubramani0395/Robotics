#include <my_pcl_tutorial/newTutorial.hpp>


ros::Publisher pub, pubfiltered, pubfiltered2;

double euclideanDistance( double x1, double y1, double x2, double y2 )
{
    double distance;
    
    distance = sqrt( pow( ( x2 - x1 ), 2 ) + pow( ( y2 - y1 ), 2 ) ) ;
    return distance ; 
}

void convertRosPclToPCl2(const sensor_msgs::PointCloud &rosPCl, pcl::PointCloud<pcl::PointXYZ> &cloud)
{
    sensor_msgs::PointCloud2 bridgePcl;
    sensor_msgs::convertPointCloudToPointCloud2(rosPCl, bridgePcl);
    pcl::moveFromROSMsg(bridgePcl, cloud);
}

void convertPcl2ToRosPcl(const pcl::PointCloud<pcl::PointXYZ> &cloud, sensor_msgs::PointCloud &rosPCl)
{
    sensor_msgs::PointCloud2 bridgePcl;
    pcl::toROSMsg(cloud, bridgePcl);
    sensor_msgs::convertPointCloud2ToPointCloud(bridgePcl, rosPCl);
}

geometry_msgs::Point32 calculateCentroid(pcl::PointCloud<pcl::PointXYZ>::Ptr oneClusterPointCloud)
{
    Eigen::Vector4f centroid;
    pcl::compute3DCentroid(*oneClusterPointCloud, centroid);
    geometry_msgs::Point32 centroidPose;
    centroidPose.x = centroid[0];
    centroidPose.y = centroid[1];
    centroidPose.z = centroid[2];
    return centroidPose;
}


void heightPassThroughFilter(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered)
{
    // Create the filtering object
    pcl::PassThrough<pcl::PointXYZ> pass;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered2;
    for (uint i = 0; i < cloud->points.size() ; i++)
    {
        
        
        if(cloud->points[i].x > -1 && cloud->points[i].x < 1  && cloud->points[i].y > -1 && cloud->points[i].y < 1 ) 
        {
            cloud_filtered->push_back(cloud->points[i]);
        }
    }
}

void useClusterExtraction(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster)
{
    std::vector<geometry_msgs::Point32> centroidVector ;
    sensor_msgs::PointCloud globalMsg;
    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
    tree->setInputCloud(cloud);
    std::vector<pcl::PointIndices> cluster_indices;
    pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
    ec.setClusterTolerance(0.15); // (tan 1.875)*10
    ec.setMinClusterSize(3);
    ec.setMaxClusterSize(25);
    ec.setSearchMethod(tree);
    ec.setInputCloud(cloud);
    ec.extract(cluster_indices);
    // Calculate centroid from clusters
    for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin(); it != cluster_indices.end(); ++it)
    {
        pcl::PointCloud<pcl::PointXYZ>::Ptr oneClusterPointCloud(new pcl::PointCloud<pcl::PointXYZ>);
        for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); ++pit)
        {
            oneClusterPointCloud->points.push_back(cloud->points[*pit]);
        }
        geometry_msgs::Point32 reflectorPose;
        
        reflectorPose = calculateCentroid(oneClusterPointCloud);      
        centroidVector.push_back(reflectorPose); 
    }
    for (std::vector<geometry_msgs::Point32>::const_iterator pit = centroidVector.begin(); pit != centroidVector.end(); ++pit)
    {
        pcl::PointXYZ reflectorPoseXYZ;
        for(int i = 0; i < centroidVector.size();  i++)
        {
            
        }

        reflectorPoseXYZ.x = pit->x;
        reflectorPoseXYZ.y = pit->y;
        reflectorPoseXYZ.z = pit->z;
        cloud_cluster->points.push_back(reflectorPoseXYZ);
    }
    //Final Rack legs Points found and published
    if(cloud_cluster->points.size() != 0 )
    {
        convertPcl2ToRosPcl(*cloud_cluster, globalMsg);
        globalMsg.header.frame_id = "cloud2";
        globalMsg.header.stamp = ros::Time::now();
        pubfiltered.publish(globalMsg);
    }
    
    
}

void scan_cb(const sensor_msgs::LaserScan::ConstPtr &input)
{
    float reading;
    laser_geometry::LaserProjection projector_;
    sensor_msgs::PointCloud globalMsg, cloud_filteredMsg;
    sensor_msgs::PointCloud2 cloud;
    sensor_msgs::PointCloud out_pointcloud;

    //Convert into point cloud
    projector_.projectLaser(*input, cloud);

    //Convert into point cloud 2 to point cloud
    sensor_msgs::convertPointCloud2ToPointCloud(cloud, out_pointcloud);
    cloud.header.frame_id = "cloud2";
    cloud.header.stamp = ros::Time::now();
    pub.publish(cloud);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_clustered(new pcl::PointCloud<pcl::PointXYZ>); 
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudxyz(new pcl::PointCloud<pcl::PointXYZ>);

    //Convert ROS PCL into PCL2
    convertRosPclToPCl2(out_pointcloud, *cloudxyz);

    //Filter Cloud with Dimensions for 1 m
    heightPassThroughFilter(cloudxyz, cloud_filtered);
    convertPcl2ToRosPcl(*cloud_filtered, cloud_filteredMsg);
    cloud_filteredMsg.header.frame_id = "cloud2";
    cloud_filteredMsg.header.stamp = ros::Time::now();
    pubfiltered2.publish(cloud_filteredMsg);

    //Filter cluster and find centroid
    useClusterExtraction(cloud_filtered, cloud_clustered);
}

int main(int argc, char **argv)
{ 
    // Initialize ROS

    ros::init(argc, argv, "rack");
    ros::NodeHandle nh;

    // Create a ROS subscriber for the input point cloud
    ros::Subscriber sub = nh.subscribe("scan", 1, scan_cb);

    // Create a ROS publisher for the output point cloud
    pub = nh.advertise<sensor_msgs::PointCloud2>("scan_cloud", 1);
    pubfiltered = nh.advertise<sensor_msgs::PointCloud>("scan_cloud1", 1);
    pubfiltered2 = nh.advertise<sensor_msgs::PointCloud>("filter_cloud2", 1);
    //image_pub = nh.advertise<sensor_msgs::Image> ("map", 1);

    // Spin
    ros::spin();
}
