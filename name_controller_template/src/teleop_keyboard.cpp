// =============================================================================
// Programming Assignment #2 — teleop_keyboard skeleton
//
// You must implement a keyboard teleop that:
//   - Subscribes to /cyclops/control_state and /cyclops/RPYD (spec §3.1)
//   - Publishes to /cyclops/command and /cyclops/control_target (spec §3.2)
//   - Refreshes the terminal at 100 Hz with std::system("clear") + ROS_INFO
//     (spec §3.5)
//   - Implements the key bindings in spec §3.3 (target adjust) and §3.4
//     (command keys)
//
// The POSIX keyboard scaffolding (init_keyboard, _kbhit, _getch) is provided
// so you can focus on the ROS pub/sub plumbing, not on termios.
// EVERY callback body and the key dispatcher are intentionally empty.
// =============================================================================

#include "ros/ros.h"

#include <std_msgs/UInt8.h>
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/MultiArrayDimension.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

// -----------------------------------------------------------------------------
// POSIX keyboard scaffolding (provided -- DO NOT change unless required).
// -----------------------------------------------------------------------------
static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard()
{
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN]  = 1;
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
    if (peek_character != -1) return 1;
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0, &ch, 1);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    if (nread == 1) { peek_character = ch; return 1; }
    return 0;
}

int _getch()
{
    char ch;
    if (peek_character != -1) {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0, &ch, 1);
    return ch;
}

// -----------------------------------------------------------------------------
// State held locally for terminal display
// -----------------------------------------------------------------------------
uint8_t control_state = 0;
uint8_t yaw_control   = 0;
uint8_t depth_control = 0;

float current_roll  = 0.0f;
float current_pitch = 0.0f;
float current_yaw   = 0.0f;
float current_depth = 0.0f;

void stateCb(const std_msgs::UInt8MultiArray::ConstPtr &msg)
{
    // TODO: read msg->data[0..2] into control_state / yaw_control / depth_control.
}

void rpydCb(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    // TODO: read msg->data[0..3] into current_roll / pitch / yaw / depth.
}

// =============================================================================
int main(int argc, char **argv)
{
    ros::init(argc, argv, "teleop_keyboard");
    ros::NodeHandle nh;

    // Subscribers (spec §3.1)
    ros::Subscriber sub_state = nh.subscribe("/cyclops/control_state", 100, stateCb);
    ros::Subscriber sub_rpyd  = nh.subscribe("/cyclops/RPYD",          100, rpydCb);

    // Publishers (spec §3.2)
    ros::Publisher pub_command = nh.advertise<std_msgs::UInt8>("/cyclops/command", 100);
    ros::Publisher pub_target  = nh.advertise<std_msgs::Float32MultiArray>("/cyclops/control_target", 100);

    // /cyclops/control_target -- 3 fields per spec §2.5
    std_msgs::Float32MultiArray target_msg;
    target_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    target_msg.layout.dim[0].label  = "control_target";
    target_msg.layout.dim[0].size   = 3;
    target_msg.layout.dim[0].stride = 3;
    target_msg.data.resize(3);
    target_msg.data[0] = 0.0f;   // target yaw   (rad)
    target_msg.data[1] = 0.0f;   // target depth (m)
    target_msg.data[2] = 0.0f;   // velocity     (unitless)

    std_msgs::UInt8 command_msg;

    ros::Rate loop_rate(100); // 100 Hz refresh per spec §3.5

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
        ROS_INFO(" Target Yaw                   =\t\t%f", target_msg.data[0]);
        ROS_INFO(" Current Depth                =\t\t%f", current_depth);
        ROS_INFO(" Target Depth                 =\t\t%f", target_msg.data[1]);
        ROS_INFO(" Vehicle Velocity             =\t\t%f", target_msg.data[2]);

        if (_kbhit())
        {
            int ch = _getch();

            // =================================================================
            // TODO: implement the key dispatch table per spec §3.3 + §3.4.
            //
            // Command keys (publish std_msgs::UInt8 on /cyclops/command):
            //   w -> 1 (forward)        y -> 6 (yaw control ON)
            //   s -> 2 (backward)       h -> 7 (yaw control OFF)
            //   a -> 3 (sway left)      p -> 8 (depth control ON)
            //   d -> 4 (sway right)     ; -> 9 (depth control OFF)
            //   q -> 5 (stop)           g -> 10 (stop all thrusters)
            //                           n -> 11 (yaw initialization)
            //
            // Target-adjust keys (modify target_msg, then publish on
            //                     /cyclops/control_target):
            //   i -> target_msg.data[0] += 0.1   (yaw +)
            //   k -> target_msg.data[0] -= 0.1   (yaw -)
            //   o -> target_msg.data[1] += 0.1   (depth +)
            //   l -> target_msg.data[1] -= 0.1   (depth -)
            //   z -> target_msg.data[2] += 10    (velocity +)
            //   x -> target_msg.data[2] -= 10    (velocity -)
            //
            // Suggested pattern:
            //   if (ch == 'w') { command_msg.data = 1; pub_command.publish(command_msg); }
            //   else if (ch == 'i') { target_msg.data[0] += 0.1; pub_target.publish(target_msg); }
            //   ...
            // =================================================================
        }

        ros::spinOnce();
        loop_rate.sleep();
    }
    close_keyboard();
    return 0;
}
