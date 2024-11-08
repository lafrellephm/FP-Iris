#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <vector>
#include <iostream>

#include "FP_Magang/PC2BS.h"
#include "FP_Magang/BS2PC.h"
using namespace std;
using namespace ros;
using namespace cv;

void imageCallback(const sensor_msgs::ImageConstPtr& msg, ros::Publisher& koord_pub) {
    try {

        Mat image = cv_bridge::toCvShare(msg, "bgr8")->image;

        resize(image, image, Size(900, 600));

        Mat hsvImage;
        cvtColor(image, hsvImage, COLOR_BGR2HSV);
        Mat mask;
        inRange(hsvImage, Scalar(5, 100, 100), Scalar(15, 255, 255), mask); 

        vector<vector<Point>> kontur;
        findContours(mask, kontur, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (!kontur.empty()) {

            auto ball = max_element(kontur.begin(), kontur.end(),
                                    [](const vector<Point>& c1, const vector<Point>& c2) {
                                        return contourArea(c1) < contourArea(c2);
                                    });

            Moments M = moments(*ball);
            int bola_x = static_cast<int>(M.m10 / M.m00);
            int bola_y = static_cast<int>(M.m01 / M.m00);

            bola_x = std::min(std::max(bola_x, 0), 600);
            bola_y = std::min(std::max(bola_y, 0), 900);

            circle(image, Point(bola_x, bola_y), 5, Scalar(0, 255, 0), -1); 
            string coord_text = "(" + to_string(bola_x) + ", " + to_string(bola_y) + ")";
            putText(image, coord_text, Point(bola_x + 10, bola_y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);

            FP_Magang::PC2BS coord_msg;
            coord_msg.bola_x = bola_x;
            coord_msg.bola_y = bola_y;
            koord_pub.publish(coord_msg);

            ROS_INFO("Detected ball coordinates: X=%d, Y=%d", bola_x, bola_y);
        }

        imshow("Coordinate System with Ball Detection", image);
        waitKey(1);

    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "node2");
    ros::NodeHandle nh;

    ros::Publisher koord_pub = nh.advertise<FP_Magang::PC2BS>("/koordinatBola", 10);

    ros::Subscriber image_sub = nh.subscribe<sensor_msgs::Image>("/gambarBola", 10,
        boost::bind(imageCallback, _1, boost::ref(koord_pub)));

    ros::spin();
    return 0;
}
