#include <my_pcl_tutorial/planeFitter.hpp>

planeFitter::planeFitter(ros::NodeHandle nh): _nh(nh){
    initialize();
}
double point2planedistnace(pcl::PointXYZ pt, pcl::ModelCoefficients::Ptr coefficients){
    double f1 = fabs(coefficients->values[0]*pt.x+coefficients->values[1]*pt.y+coefficients->values[2]*pt.z+coefficients->values[3]);
    double f2 = sqrt(pow(coefficients->values[0],2)+pow(coefficients->values[1],2)+pow(coefficients->values[2],2));
    return f1/f2;
}
void planeFitter::initialize(){

    // Get node name
    _name = ros::this_node::getName();

    // Get Publisher read
    _pub_inliers = _nh.advertise< sensor_msgs::PointCloud2 >("inliers",2);

    // Subscriber
    _subs = _nh.subscribe("/velodyne_points",1,&planeFitter::pointCloudCb,this);

    // Get parameters
    ros::param::param<double>("~max_distance",_max_distance,0.005);
    ros::param::param<double>("~min_percentage",_min_percentage,5);
    ros::param::param<bool>("~color_pc_with_error",_color_pc_with_error,false);

    // Create dynamic reconfigure
    drCallback = boost::bind( &planeFitter::updateParameters, this, _1, _2);
    dRserver.setCallback(drCallback);

    // Create colors pallete
    createColors();

    // Inform initialized
    ROS_INFO("%s: node initialized.",_name.c_str());
}

void planeFitter::updateParameters(plane_fitter::algorithmParametersConfig& config, uint32_t level){
    _max_distance = config.max_distance/1000;
    _min_percentage = config.min_percentage_of_points;
    _color_pc_with_error = config.paint_with_error;
}

void planeFitter::pointCloudCb(const sensor_msgs::PointCloud2::ConstPtr &msg){

    // Convert to pcl point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_msg (new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud (new pcl::PointCloud<pcl::PointXYZ>);
    pcl::fromROSMsg(*msg,*cloud_msg);
    ROS_DEBUG("%s: new ponitcloud (%i,%i)(%zu)",_name.c_str(),cloud_msg->width,cloud_msg->height,cloud_msg->size());

    // Filter cloud
    pcl::PassThrough<pcl::PointXYZ> pass;
    Eigen::Matrix4f transform_1 = Eigen::Matrix4f::Identity();
    transform_1 (2,3) = 1.9;
    
    pcl::transformPointCloud (*cloud_msg, *transformed_cloud, transform_1);
    //cout<<transform_1.matrix()<<endl;
    pass.setInputCloud(transformed_cloud);
    pass.setFilterFieldName ("z");
    pass.setFilterLimits(0.001,10000);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
    pass.filter (*cloud);

    // Get segmentation ready
    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    seg.setOptimizeCoefficients (true);
    seg.setModelType (pcl::SACMODEL_PLANE);
    seg.setMethodType (pcl::SAC_RANSAC);
    seg.setDistanceThreshold(_max_distance);

    // Create pointcloud to publish inliers
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_pub(new pcl::PointCloud<pcl::PointXYZRGB>);
    int original_size(cloud->height*cloud->width);
    int n_planes(0);
    while (cloud->height*cloud->width>original_size*_min_percentage/100){

        // Fit a plane
        seg.setInputCloud(cloud);
        seg.segment(*inliers, *coefficients);

        // Check result
        if (inliers->indices.size() == 0)
            break;

        // Iterate inliers
        double mean_error(0);
        double max_error(0);
        double min_error(100000);
        std::vector<double> err;
        for (int i=0;i<inliers->indices.size();i++){

            // Get Point
            pcl::PointXYZ pt = cloud->points[inliers->indices[i]];

            // Compute distance
            double d = point2planedistnace(pt,coefficients)*1000;// mm
            err.push_back(d);

            // Update statistics
            mean_error += d;
            if (d>max_error) max_error = d;
            if (d<min_error) min_error = d;

        }
        mean_error/=inliers->indices.size();

        // Compute Standard deviation
        ColorMap cm(min_error,max_error);
        double sigma(0);
        for (int i=0;i<inliers->indices.size();i++){

            sigma += pow(err[i] - mean_error,2);

            // Get Point
            pcl::PointXYZ pt = cloud->points[inliers->indices[i]];

            // Copy point to noew cloud
            pcl::PointXYZRGB pt_color;
            pt_color.x = pt.x;
            pt_color.y = pt.y;
            pt_color.z = pt.z;
            uint32_t rgb;
            if (_color_pc_with_error)
                rgb = cm.getColor(err[i]);
            else
                rgb = colors[n_planes].getColor();
            pt_color.rgb = *reinterpret_cast<float*>(&rgb);
            cloud_pub->points.push_back(pt_color);

        }
        sigma = sqrt(sigma/inliers->indices.size());

        // Extract inliers
        extract.setInputCloud(cloud);
        extract.setIndices(inliers);
        extract.setNegative(true);
        pcl::PointCloud<pcl::PointXYZ> cloudF;
        extract.filter(cloudF);
        cloud->swap(cloudF);

        // Display infor
        ROS_INFO("%s: fitted plane %i: %fx%s%fy%s%fz%s%f=0 (inliers: %zu/%i)",
                    _name.c_str(),n_planes,
                    coefficients->values[0],(coefficients->values[1]>=0?"+":""),
                    coefficients->values[1],(coefficients->values[2]>=0?"+":""),
                    coefficients->values[2],(coefficients->values[3]>=0?"+":""),
                    coefficients->values[3],
                    inliers->indices.size(),original_size);
        ROS_INFO("%s: mean error: %f(mm), standard deviation: %f (mm), max error: %f(mm)",_name.c_str(),mean_error,sigma,max_error);
        ROS_INFO("%s: poitns left in cloud %i",_name.c_str(),cloud->width*cloud->height);
       
        // Nest iteration
        n_planes++;
    }

    // Publish points
    sensor_msgs::PointCloud2 cloud_publish;
    pcl::toROSMsg(*cloud_pub,cloud_publish);
    cloud_publish.header = msg->header;
    _pub_inliers.publish(cloud_publish);
}

void planeFitter::createColors(){
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    for (int i=0;i<20;i++){
        while (r<70 && g < 70 && b < 70){
            r = rand()%(255);
            g = rand()%(255);
            b = rand()%(255);
        }
        Color c(r,g,b);
        r = 0;
        g = 0;
        b = 0;
        colors.push_back(c);
    }
}

void planeFitter::spin(){
    ros::spin();
}


