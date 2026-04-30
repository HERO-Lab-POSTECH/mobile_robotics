#include "ros/ros.h"
#include <std_msgs/Char.h>
#include <sensor_msgs/FluidPressure.h>

#include "uuv_gazebo_ros_plugins_msgs/FloatStamped.h"


int control_state = 0;
int control_speed = 30;

float depth = 0;

void msgCallback(const std_msgs::Char::ConstPtr &msg)
{
    char command = msg->data; // pointer used in ros call back function

    ROS_INFO("Input: %c", command);

    // ================================================================
    // TODO 1: Map keyboard commands to control_state / control_speed.
    //
    //   Each press of a key sets the variable below to a number. The
    //   while-loop in main() then reads that number and decides which
    //   thrusters to drive (TODO 3).
    //
    //   Key -> behaviour:
    //     'q' : stop          (control_state = 0)   <-- given
    //     'w' : forward       (control_state = 1)   <-- given as a 2nd example
    //     's' : backward      (control_state = 2)
    //     'a' : sway-left     (control_state = 3)
    //     'd' : sway-right    (control_state = 4)
    //     'i' : heave-up      (control_state = 5)
    //     'k' : heave-down    (control_state = 6)
    //     'z' : control_speed += 10
    //     'x' : control_speed -= 10
    //
    //   Just follow the pattern of the two given examples. For 'z' and
    //   'x' you don't change control_state -- you change control_speed
    //   instead (use += and -=).
    // ================================================================
    if      (command == 'q') control_state = 0;        // stop          (worked example)
    else if (command == 'w') control_state = 1;        // forward       (worked example)
    // else if (command == 's') control_state = ?;     // TODO: backward
    // else if (command == 'a') control_state = ?;     // TODO: sway-left
    // else if (command == 'd') control_state = ?;     // TODO: sway-right
    // else if (command == 'i') control_state = ?;     // TODO: heave-up
    // else if (command == 'k') control_state = ?;     // TODO: heave-down
    // else if (command == 'z') control_speed += ?;    // TODO: speed up
    // else if (command == 'x') control_speed -= ?;    // TODO: speed down
}

void msgCallback_depth(const sensor_msgs::FluidPressure::ConstPtr &msg)
{
    depth = ((msg->fluid_pressure / 101.325) - 1) * 10;
    float print_depth = depth;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "control_node");
    ros::NodeHandle nh;
    ros::Rate loop_rate(100);

    //
    // Subscribers (already provided)
    //
    ros::Subscriber sub = nh.subscribe("/key_command", 100, msgCallback);
    ros::Subscriber sub_depth = nh.subscribe("/cyclops/pressure", 100, msgCallback_depth);

    // ================================================================
    // TODO 2: Declare 8 thruster publishers (pub_thruster0 .. pub_thruster7).
    //
    //   Each thruster has its own topic:
    //     thruster 0  ->  /cyclops/thrusters/0/input
    //     thruster 1  ->  /cyclops/thrusters/1/input
    //         ...
    //     thruster 7  ->  /cyclops/thrusters/7/input
    //
    //   They all use the same message type:
    //     uuv_gazebo_ros_plugins_msgs::FloatStamped
    //   and queue size 100.
    //
    //   Two are given so you can see the exact pattern. Replace each
    //   stub below with a real nh.advertise<...>(...) line that follows
    //   the same shape -- only the number changes.
    //
    //   See Lab 06 slide p18.
    // ================================================================
    ros::Publisher pub_thruster0 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/0/input", 100); // worked example
    ros::Publisher pub_thruster1 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/1/input", 100); // worked example
    ros::Publisher pub_thruster2;  // TODO 2: replace with nh.advertise<...>("/cyclops/thrusters/2/input", 100);
    ros::Publisher pub_thruster3;  // TODO 2: same pattern, thruster 3
    ros::Publisher pub_thruster4;  // TODO 2: same pattern, thruster 4
    ros::Publisher pub_thruster5;  // TODO 2: same pattern, thruster 5
    ros::Publisher pub_thruster6;  // TODO 2: same pattern, thruster 6
    ros::Publisher pub_thruster7;  // TODO 2: same pattern, thruster 7

    uuv_gazebo_ros_plugins_msgs::FloatStamped vel;

    while (ros::ok())
    {
        ROS_INFO("depth = %f", depth);

        // ============================================================
        // TODO 3: Drive the 8 thrusters according to control_state.
        //
        //   Two branches are given as worked examples:
        //     - 'forward' (control_state == 1)
        //     - 'stop'    (control_state == 0)
        //
        //   You must write the remaining 5 branches:
        //     control_state == 2 (backward)
        //     control_state == 3 (sway-left)
        //     control_state == 4 (sway-right)
        //     control_state == 5 (heave-up)
        //     control_state == 6 (heave-down)
        //
        //   Pattern to follow (copied from the 'forward' example):
        //     1) set vel.data to the desired thrust value
        //        (positive or negative * control_speed * 10)
        //     2) call pub_thrusterN.publish(vel) on every thruster you
        //        want to drive
        //
        //   Tips for each direction (see slide p19 for exact thrust signs):
        //     backward     : like forward but flip the sign on thrusters 2 & 3.
        //     sway-left/-right : drive thrusters 4..7, leave 2/3 at 0.
        //     heave-up/-down   : drive thrusters 0 & 1, leave 2..7 at 0.
        // ============================================================
        if (control_state == 1) // forward (worked example)
        {
            vel.data = -control_speed * 10;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);

            vel.data = 0;
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 0) // stop (worked example)
        {
            vel.data = 0;
            pub_thruster0.publish(vel);
            pub_thruster1.publish(vel);
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        // else if (control_state == 2) // TODO: backward
        // {
        //     vel.data = +control_speed * 10;       // surge thrusters: opposite sign of forward
        //     pub_thruster2.publish(vel);
        //     pub_thruster3.publish(vel);
        //
        //     vel.data = 0;
        //     pub_thruster4.publish(vel);
        //     pub_thruster5.publish(vel);
        //     pub_thruster6.publish(vel);
        //     pub_thruster7.publish(vel);
        // }
        // else if (control_state == 3) // TODO: sway-left
        // {
        //     // Hint: thrusters 4 & 5 push one way, 6 & 7 push the opposite way,
        //     //       thrusters 2 & 3 stay at 0. See slide p19 for signs.
        // }
        // else if (control_state == 4) // TODO: sway-right
        // {
        //     // Hint: same idea as sway-left but flip every sign on 4..7.
        // }
        // else if (control_state == 5) // TODO: heave-up
        // {
        //     // Hint: drive thrusters 0 & 1; thrusters 2..7 stay at 0.
        // }
        // else if (control_state == 6) // TODO: heave-down
        // {
        //     // Hint: same as heave-up but flip the sign on thrusters 0 & 1.
        // }

        loop_rate.sleep();
        ros::spinOnce();
    }

    return 0;
}
