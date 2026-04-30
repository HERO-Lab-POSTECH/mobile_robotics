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
    // TODO 1: Map the keyboard command to control_state / control_speed.
    //   See Lab06 slide p17 for the full mapping.
    //   Required keys:
    //     'q' : stop          (control_state = 0)
    //     'w' : forward       (control_state = 1)
    //     's' : backward      (control_state = 2)
    //     'a' : sway-left     (control_state = 3)
    //     'd' : sway-right    (control_state = 4)
    //     'i' : heave-up      (control_state = 5)
    //     'k' : heave-down    (control_state = 6)
    //     'z' : control_speed += 10
    //     'x' : control_speed -= 10
    //   The 'q' case is provided as a worked example. Fill in the rest.
    // ================================================================
    if (command == 'q') control_state = 0;
    // else if (command == 'w') ...
    // else if (command == 's') ...
    // else if (command == 'a') ...
    // else if (command == 'd') ...
    // else if (command == 'i') ...
    // else if (command == 'k') ...
    // else if (command == 'z') ...
    // else if (command == 'x') ...
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
    //   - Topic name   : /cyclops/thrusters/<n>/input  (n = 0..7)
    //   - Message type : uuv_gazebo_ros_plugins_msgs::FloatStamped
    //   - Queue size   : 100
    //   See Lab06 slide p18 for the syntax.
    //   Replace the stubs below with real nh.advertise<...>(...) calls.
    // ================================================================
    ros::Publisher pub_thruster0;  // <-- stub; assign with nh.advertise<...>(...)
    ros::Publisher pub_thruster1;
    ros::Publisher pub_thruster2;
    ros::Publisher pub_thruster3;
    ros::Publisher pub_thruster4;
    ros::Publisher pub_thruster5;
    ros::Publisher pub_thruster6;
    ros::Publisher pub_thruster7;

    uuv_gazebo_ros_plugins_msgs::FloatStamped vel;

    while (ros::ok())
    {
        ROS_INFO("depth = %f", depth);

        // ============================================================
        // TODO 3: Drive the 8 thrusters according to control_state.
        //   See Lab06 slide p19 for the thruster mapping.
        //   The 'forward' case (control_state == 1) is provided as a
        //   worked example. Fill in backward / sway-left / sway-right /
        //   heave-up / heave-down / stop following the same pattern.
        //
        //   Tip:  vel.data = <signed thrust>;
        //         pub_thrusterN.publish(vel);
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
        // else if (control_state == 2) // backward
        // {
        //     ...
        // }
        // else if (control_state == 3) // sway-left
        // {
        //     ...
        // }
        // else if (control_state == 4) // sway-right
        // {
        //     ...
        // }
        // else if (control_state == 5) // heave-up
        // {
        //     ...
        // }
        // else if (control_state == 6) // heave-down
        // {
        //     ...
        // }
        // else if (control_state == 0) // stop
        // {
        //     vel.data = 0;
        //     pub_thruster0.publish(vel); ...  pub_thruster7.publish(vel);
        // }

        loop_rate.sleep();
        ros::spinOnce();
    }

    return 0;
}
