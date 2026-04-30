#include "ros/ros.h"
#include <std_msgs/UInt8.h>
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/MultiArrayDimension.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <termios.h>

using namespace std;

// for keyboard input
static struct termios initial_settings, new_settings;

static int peek_character = -1;

void init_keyboard();
void close_keyboard();
int _kbhit();
int _getch();
int _putch(int c);
////////////////////////

uint8_t control_state = 0;
uint8_t yaw_control = 0;
uint8_t depth_control = 0;
void msgCallback_state(const std_msgs::UInt8MultiArray::ConstPtr &msg)
{
    control_state = msg->data[0];
    yaw_control = msg->data[1];
    depth_control = msg->data[2];
}

float current_roll = 0.0;
float current_pitch = 0.0;
float current_yaw = 0.0;
float current_depth = 0.0;
void msgCallback_sensor(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    current_roll = msg->data[0];
    current_pitch = msg->data[1];
    current_yaw = msg->data[2];
    current_depth = msg->data[3];
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "teleop_keyboard");
    ros::NodeHandle nh;

    ros::Subscriber state_sub =
        nh.subscribe("/cyclops/control_state", 100, msgCallback_state);
    ros::Subscriber sensor_sub =
        nh.subscribe("/cyclops/RPYD", 100, msgCallback_sensor);

    ros::Publisher command_pub = nh.advertise<std_msgs::UInt8>("/cyclops/command", 100);
    ros::Publisher con_target_pub = nh.advertise<std_msgs::Float32MultiArray>("/cyclops/control_target", 100);

    std_msgs::UInt8 command_msg;
    std_msgs::Float32MultiArray con_target_msg;
    con_target_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    con_target_msg.layout.dim[0].label = "control_target";
    con_target_msg.layout.dim[0].size = 5;
    con_target_msg.layout.dim[0].stride = 5;
    con_target_msg.layout.data_offset = 5;
    // 1 channel, size 5 (tartget yaw, target depth, target velocity, target_pos_x, target_pos_y)
    con_target_msg.data.resize(5);
    con_target_msg.data[0] = 0.0;   //target yaw 
    con_target_msg.data[1] = 55.0;   //target depth
    con_target_msg.data[2] = 250.0;   //vehicle velocity
    con_target_msg.data[3] = 0; //position x 
    con_target_msg.data[4] = 0; //position y

    ros::Rate loop_rate(100);

    init_keyboard();

    while (ros::ok())
    {
        std::system("clear");
        
        ROS_INFO(" Current Control State        =\t\t%d", control_state);  
        ROS_INFO(" Current Yaw Control State    =\t\t%d", yaw_control);
        ROS_INFO(" Current Depth Control State  =\t\t%d", depth_control);   
        ROS_INFO(" Current Roll                 =\t\t%f", current_roll);  
        ROS_INFO(" Current Pitch                =\t\t%f", current_pitch);  
        ROS_INFO(" Current Yaw                  =\t\t%f", current_yaw);                  
        ROS_INFO(" Target Yaw                   =\t\t%f", con_target_msg.data[0]);    
        ROS_INFO(" Current Depth                =\t\t%f", current_depth);                  
        ROS_INFO(" Target Depth                 =\t\t%f", con_target_msg.data[1]);               
        ROS_INFO(" Vehicle Velocity             =\t\t%f", con_target_msg.data[2]);
        ROS_INFO(" Target Position x            =\t\t%f", con_target_msg.data[3]);
        ROS_INFO(" Target Position y            =\t\t%f", con_target_msg.data[4]);

        if (_kbhit())
        {
            int ch = _getch();

            //_putch(ch);
            // cyclops manually control part 

            if (ch == 'w') // forward
            {
                command_msg.data = 1;
                command_pub.publish(command_msg);
            }
            else if (ch == 's') // backward
            {
                command_msg.data = 2;
                command_pub.publish(command_msg);
            }
            else if (ch == 'a') // sway left
            {
                command_msg.data = 3;
                command_pub.publish(command_msg);
            }
            else if (ch == 'd') // sway right
            {
                command_msg.data = 4;
                command_pub.publish(command_msg);
            }
            
            // cyclops pid control part
            else if (ch == '8') // forward (pid x+)
            {
                command_msg.data = 12;
                command_pub.publish(command_msg);
                con_target_msg.data[3] += 0.5; // position x++ 1m unit
                con_target_pub.publish(con_target_msg);

            }
            else if (ch == '5') // forward (pid x-)
            {
                command_msg.data = 13;
                command_pub.publish(command_msg);
                con_target_msg.data[3] -= 0.5; // position x++ 1m unit
                con_target_pub.publish(con_target_msg);

            }
            else if (ch == '6') // forward (pid y+)
            {
                command_msg.data = 14;
                command_pub.publish(command_msg);
                con_target_msg.data[4] += 0.5; // position x++ 1m unit
                con_target_pub.publish(con_target_msg);

            }
            else if (ch == '4') // forward (pid y-)
            {
                command_msg.data = 15;
                command_pub.publish(command_msg);
                con_target_msg.data[4] -= 0.5; // position x++ 1m unit
                con_target_pub.publish(con_target_msg);

            }                                    
            
            else if (ch == 'q') // stop
            {
                command_msg.data = 5;
                command_pub.publish(command_msg);
            }
            else if (ch == 'y') // yaw control on
            {
                command_msg.data = 6;
                command_pub.publish(command_msg);
            }
            else if (ch == 'h') // yaw control off
            {
                command_msg.data = 7;
                command_pub.publish(command_msg);
            }
            else if (ch == 'p') // depth control on
            {
                command_msg.data = 8;
                command_pub.publish(command_msg);
            }
            else if (ch == ';') // depth control off
            {
                command_msg.data = 9;
                command_pub.publish(command_msg);
            }
            else if (ch == 'g') // all truster stop
            {
                command_msg.data = 10;
                command_pub.publish(command_msg);
            }
            else if (ch == 'n') // init 0: yaw sensor value
            {
                command_msg.data = 11;
                command_pub.publish(command_msg);
            }
            else if (ch == 'i') // yaw ++ 0.1
            {
                con_target_msg.data[0] += 0.1;
                con_target_pub.publish(con_target_msg);
            }
            else if (ch == 'k') // yaw -- 0.1
            {
                con_target_msg.data[0] -= 0.1;
                con_target_pub.publish(con_target_msg);
            }
            else if (ch == 'o') // depth ++ 0.1
            {
                con_target_msg.data[1] += 0.1;
                con_target_pub.publish(con_target_msg);
            }
            else if (ch == 'l') // depth -- 0.1
            {
                con_target_msg.data[1] -= 0.1;
                con_target_pub.publish(con_target_msg);
            }
            else if (ch == 'z') // velocity ++ 0.1
            {
                con_target_msg.data[2] += 0.1;
                con_target_pub.publish(con_target_msg);
            }
            else if (ch == 'x') // velocity -- 0.1
            {
                con_target_msg.data[2] -= 0.1;
                con_target_pub.publish(con_target_msg);
            }

            ROS_INFO("Published input: %c", ch);
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    close_keyboard();

    return 0;
}

void init_keyboard()
{
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}

int _kbhit()
{
    unsigned char ch;
    int nread;

    if (peek_character != -1)
        return 1;

    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0, &ch, 1);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);

    if (nread == 1)
    {
        peek_character = ch;
        return 1;
    }

    return 0;
}

int _getch()
{
    char ch;

    if (peek_character != -1)
    {
        ch = peek_character;
        peek_character = -1;

        return ch;
    }
    read(0, &ch, 1);

    return ch;
}

int _putch(int c)
{
    putchar(c);
    fflush(stdout);

    return c;
}
