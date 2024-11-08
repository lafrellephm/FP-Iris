#include <ros/ros.h>
#include <cmath>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include "FP_Magang/PC2BS.h"
#include "FP_Magang/BS2PC.h"

float x = 0.0;
float y = 0.0;
float theta = 0.0; 
float bola_x = 0.0;
float bola_y = 0.0;

float motor1 = 0.0; 
float motor2 = 0.0; 
float motor3 = 0.0; 
bool bawaBola = false;

void setupTerminal(struct termios& old_tio) {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restoreTerminal(struct termios& old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}

void v_Omniwheel(float &vx, float &vy, float &vth) {

    float V1 = motor1; 
    float V2 = motor2;  
    float V3 = motor3; 

    vx = (1.0 / 3.0) * (V1 + V2 - V3);  
    vy = (1.0 / 3.0) * (-sqrt(3) * V2 + sqrt(3) * V3);  
    vth = (1.0 / 3.0) * (V1 - V2 + V3);  
}

void bs2pcCallback(const FP_Magang::BS2PC::ConstPtr& msg, ros::Publisher& pub) {

    float tujuan_x = msg->tujuan_x;
    float tujuan_y = msg->tujuan_y;
    float enc_left = msg->enc_left;
    float enc_right = msg->enc_right;
    float th = msg->th;

    if (th > 180.0) {
        theta = th - 360.0;
    } else {
        theta = th;
    }
 
    x += enc_left * cos(theta * M_PI / 180.0);
    y += enc_right * sin(theta * M_PI / 180.0);

    if (x < 0) x = 0;       
    if (y < 0) y = 0;      
    if (x > 600) x = 600;   
    if (y > 900) y = 900;   

    float vx, vy, vth;
    v_Omniwheel(vx, vy, vth);

    FP_Magang::PC2BS pub_msg;
    pub_msg.bola_x = bola_x;  
    pub_msg.bola_y = bola_y;  
    pub_msg.motor1 = motor1;  
    pub_msg.motor2 = motor2;  
    pub_msg.motor3 = motor3;  

    // Publish to /pc2bs topic
    pub.publish(pub_msg);

    // Log information
    ROS_INFO("Published to /pc2bs: bola_x=%.2f, bola_y=%.2f, motor1=%.2f, motor2=%.2f, motor3=%.2f, vx=%.2f, vy=%.2f, vth=%.2f",
             pub_msg.bola_x, pub_msg.bola_y, pub_msg.motor1, pub_msg.motor2, pub_msg.motor3, vx, vy, vth);
}

void koordinatBolaCallback(const FP_Magang::PC2BS::ConstPtr& msg) {
    bola_x = msg->bola_x;
    bola_y = msg->bola_y;
    ROS_INFO("Koordinat bola: bola_x=%.2f, bola_y=%.2f", bola_x, bola_y);
}

void robotMove(char command) {
    switch (command) {
        case 'w': motor1 = 0; motor2 = 20; motor3 = -20; break; 
        case 's': motor1 = 0; motor2 = -20; motor3 = 20; break; 
        case 'a': motor1 = -20; motor2 = 20; motor3 = -10; break;   
        case 'd': motor1 = 20; motor2 = -20; motor3 = -10; break; 
        case 'q': motor1 = 40; motor2 = 40; motor3 = 40; break;   
        case 'e': motor1 = -30; motor2 = -30; motor3 = -30; break; 
        case 'z': bawaBola = true; ROS_INFO("Ball picked up!"); break;
        case 'x': bawaBola = false; ROS_INFO("Ball released!"); break;
        default: motor1 = 0; motor2 = 0; motor3 = 0; break;
    }
}

int getch() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "node3");
    ros::NodeHandle nh;

    ros::Publisher pub = nh.advertise<FP_Magang::PC2BS>("/pc2bs", 10);

    ros::Subscriber sub_bs2pc = nh.subscribe<FP_Magang::BS2PC>("/bs2pc", 10,
        boost::bind(bs2pcCallback, _1, boost::ref(pub)));

    ros::Subscriber sub_koordinat_bola = nh.subscribe<FP_Magang::PC2BS>("/koordinatBola", 10, koordinatBolaCallback);

    ros::Rate rate(50);  

    while (ros::ok()) {
 
        char c = getch();
        robotMove(c);

        FP_Magang::PC2BS msg;
        msg.motor1 = motor1;
        msg.motor2 = motor2;
        msg.motor3 = motor3;

        msg.bola_x = bawaBola ? 1.0 : 0.0;
        msg.bola_y = bawaBola ? 1.0 : 0.0;

        pub.publish(msg);
        ROS_INFO("Published: motor1=%.2f, motor2=%.2f, motor3=%.2f", motor1, motor2, motor3);

        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
