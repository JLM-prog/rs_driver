#ifndef __GNUC__
#include <boost\asio.hpp>
#include <Windows.h>
#endif
#include "driver/lidar_driver.hpp"
#include "msg/lidar_points_msg.h"
#include <iostream>
#ifdef __GNUC__
#include <ros/ros.h>
#include <ros/publisher.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
ros::Publisher lidar_points_pub_;
#endif
bool start_ = true;
struct PointXYZI
{
    double x;
    double y;
    double z;
    double intensity;
};

#ifdef __GNUC__
void callback(const robosense::LidarPointsMsg<pcl::PointXYZI> &msg)
{
    sensor_msgs::PointCloud2 ros_msg;
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZI>);
    for (auto iter : *msg.cloudPtr)
    {
        cloud2->push_back(iter);
    }
    pcl::toROSMsg(*cloud2, ros_msg);

    ros_msg.header.stamp = ros_msg.header.stamp.fromSec(msg.timestamp);
    ros_msg.header.frame_id = msg.parent_frame_id;
    ros_msg.header.seq = msg.seq;

    lidar_points_pub_.publish(ros_msg);
    std::cout << "msg: " << msg.seq << std::endl;
}
#else
void callback(const robosense::LidarPointsMsg<PointXYZI> &msg)
{
    std::cout << "msg: " << msg.seq < < < < "pointcloud size: " << msg.cloudPtr->size() << std::endl;
}
#endif

int main(int argc, char *argv[])
{
#ifdef __GNUC__
    ros::init(argc, argv, "driver", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh_;
    lidar_points_pub_ = nh_.advertise<sensor_msgs::PointCloud2>("points", 10);
    std::shared_ptr<robosense::sensor::LidarDriver<pcl::PointXYZI>> demo_ptr = std::make_shared<robosense::sensor::LidarDriver<pcl::PointXYZI>>();
#else
    std::shared_ptr<robosense::sensor::LidarDriver<PointXYZI>> demo_ptr = std::make_shared<robosense::sensor::LidarDriver<PointXYZI>>();
#endif


    robosense::sensor::RSLiDAR_Driver_Param param;
#ifdef __GNUC__
    param.input_param.read_pcap = true;
#elif _MSC_VER
    param.input_param.read_pcap = TRUE;
#endif
    param.input_param.pcap_file_dir = "/media/xzd/bag/bag/sunnyvael_1014.pcap";
    param.calib_path = "/home/xzd/work/lidar_driver/conf";
    param.device_type = "RS128";
    demo_ptr->init(param);
    demo_ptr->regPointRecvCallback(callback);
    demo_ptr->start();
    while (start_)
    {
#ifdef __GNUC__
        sleep(1);
#else
        Sleep(1);
#endif
    }
}
