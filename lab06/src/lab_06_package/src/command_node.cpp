#include "ros/ros.h"
#include <std_msgs/Char.h>

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

int main(int argc, char **argv)
{
    ros::init(argc, argv, "command_node");
    ros::NodeHandle nh;

	//
	//Declare publisher that advertises key_command
	//Publisher name = ros_pub
	//Refer to instruction to set msg type and topic name 
	//Quesize = 100

    ros::Publisher ros_pub = nh.advertise<std_msgs::Char>("/key_command", 100);
    ros::Rate loop_rate(100);

    init_keyboard();
    while (ros::ok()) {
        std_msgs::Char msg;
        if(_kbhit()){
            msg.data = _getch();
            ROS_INFO("%c", msg.data);
        }
        ros_pub.publish(msg);

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