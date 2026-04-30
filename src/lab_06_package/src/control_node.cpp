#include "ros/ros.h"
#include <std_msgs/Char.h>
#include <sensor_msgs/FluidPressure.h>

#include "uuv_gazebo_ros_plugins_msgs/FloatStamped.h"


int control_state = 0;
int control_speed = 30;

float depth = 0;

void msgCallback(const std_msgs::Char::ConstPtr &msg)
{
    char command = msg->data; //pointer used in ros call back function 

    ROS_INFO("Input: %c", command);

	//Programming remaining parts to change the control_state according to the command.
	//Using ROS_INFO, inform the command and its meaning.

    if(command == 'q') control_state = 0;
	else if(command == 'w') control_state = 1;
    else if(command == 's') control_state = 2;
    else if(command == 'a') control_state = 3;
    else if(command == 'd') control_state = 4;
    else if(command == 'i') control_state = 5;
    else if(command == 'k') control_state = 6;
    else if(command == 'z') control_speed += 10;
    else if(command == 'x') control_speed -= 10;

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
	// Declare subscriber that subscribes keyboard message on ROS 
	// This subscriber includes callback function named "msgCallback".
	// Queue size is 100. 

    ros::Subscriber sub = nh.subscribe("/key_command", 100, msgCallback);
    ros::Subscriber sub_depth = nh.subscribe("/cyclops/pressure", 100, msgCallback_depth);

	//
	// Declare 8 thruster publishers that advertize output msgs. 
	// Publisher name = pub_thruster#n,and n = 0 to 7.
	// Refer to instruction to set msg type and topic name.
	// Quesize = 100
    ros::Publisher pub_thruster0 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/0/input", 100);
    ros::Publisher pub_thruster1 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/1/input", 100);
    ros::Publisher pub_thruster2 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/2/input", 100);
    ros::Publisher pub_thruster3 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/3/input", 100);
    ros::Publisher pub_thruster4 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/4/input", 100);
    ros::Publisher pub_thruster5 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/5/input", 100);
    ros::Publisher pub_thruster6 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/6/input", 100);
    ros::Publisher pub_thruster7 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/7/input", 100);
	
    uuv_gazebo_ros_plugins_msgs::FloatStamped vel;

    while (ros::ok())
    {
	ROS_INFO("depth = %f", depth);
        if (control_state == 1) //전진 forward
        {
			//
			// Declare publish message
			// ??? = declared variable name

            vel.data = -control_speed * 10;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);

            vel.data = 0;
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 2) //후진 backward
        {
			//
			// Declare publish message
            vel.data = control_speed * 10;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);

           vel.data = 0;
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 3) //좌 sway
        {
			//
			// Declare publish message
            vel.data = 0;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);

            vel.data = control_speed * 10;
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            vel.data = -control_speed * 10;
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 4) //우 sway
        {
			//
			// Declare publish message
            vel.data = 0;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);

            vel.data = -control_speed * 10;
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            vel.data = control_speed * 10;
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 5) //상승 heave up
        {
            vel.data = -control_speed * 10;
            pub_thruster0.publish(vel);
            pub_thruster1.publish(vel);

            vel.data = 0;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 6) //하강 heave down
        {
            vel.data = control_speed * 10;
            pub_thruster0.publish(vel);
            pub_thruster1.publish(vel);

            vel.data = 0;
            pub_thruster2.publish(vel);
            pub_thruster3.publish(vel);
            pub_thruster4.publish(vel);
            pub_thruster5.publish(vel);
            pub_thruster6.publish(vel);
            pub_thruster7.publish(vel);
        }
        else if (control_state == 0) //멈춤 stop
        {
			//
			// Declare publish message
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

        loop_rate.sleep();
        ros::spinOnce();
    }

    return 0;
}
