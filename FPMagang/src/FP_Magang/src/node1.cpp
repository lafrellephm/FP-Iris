#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

    ros::init(argc, argv, "node1");
    ros::NodeHandle nh;

    ros::Publisher data_pub = nh.advertise<sensor_msgs::Image>("/gambarBola", 10);

    vector<std::string> image_files = {
        "/home/lafrelle/FPMagang/src/FP_Magang/src/bola2.jpg"
    };

    ros::Rate loop_rate(1);

    size_t n = 0;
    while (ros::ok()) {

        Mat image = imread(image_files[n], IMREAD_COLOR);
        if (image.empty()) {
            ROS_ERROR("Gagal membaca gambar: %s", image_files[n].c_str());
            n = (n + 1) % image_files.size(); 
        }

        Mat image_resized;
        resize(image, image_resized, Size(900, 600));

        sensor_msgs::ImagePtr msgImg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image_resized).toImageMsg();

        data_pub.publish(msgImg);
        ROS_INFO("Published image: %s", image_files[n].c_str());

        waitKey(30);
        n = (n + 1) % image_files.size();

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
