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

    // ================================================================
    // TODO 1: Declare a publisher that advertises /key_command.
    //
    //   Required parameters:
    //     - Variable name : ros_pub
    //     - Topic name    : "/key_command"
    //     - Message type  : std_msgs::Char
    //     - Queue size    : 100
    //
    //   General syntax for nh.advertise:
    //     ros::Publisher <var> = nh.advertise< <MsgType> >(<topic>, <queue>);
    //
    //   So with the values above, your line should look like:
    //     ros::Publisher ros_pub = nh.advertise<std_msgs::Char>(   ???   ,   ???   );
    //
    //   Replace the stub line below with the complete declaration.
    //   See Lab 06 slide p13 for the same pattern.
    // ================================================================
    ros::Publisher ros_pub;  // <-- TODO 1: replace with the full nh.advertise<...>(...) line

    ros::Rate loop_rate(100);

    init_keyboard();
    while (ros::ok()) {
        std_msgs::Char msg;
        if(_kbhit()){
            msg.data = _getch();
            ROS_INFO("%c", msg.data);
        }

        // ============================================================
        // TODO 2: Publish the keyboard message on /key_command.
        //
        //   You already have the publisher (ros_pub) and the message
        //   (msg). Calling .publish() on the publisher object pushes
        //   the message onto the topic.
        //
        //   Just uncomment the next line:
        // ============================================================
        // ros_pub.publish(msg);

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
