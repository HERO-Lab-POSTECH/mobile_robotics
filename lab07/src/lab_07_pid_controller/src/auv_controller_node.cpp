#include <ros/package.h>
#include "ros/ros.h"

#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/UInt8.h>


#include <sensor_msgs/FluidPressure.h>
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>

#include "uuv_gazebo_ros_plugins_msgs/FloatStamped.h"

#define THROTTLE 250.0

#define YAW_ANGLE_GAIN_P 50.0
#define YAW_GAIN_P 150.0
#define YAW_GAIN_I 5.0
#define YAW_GAIN_D 10.0

#define DEPTH_GAIN_P 5000.0
#define DEPTH_GAIN_I 50.0
#define DEPTH_GAIN_D 500.0

#define X_GAIN_P 2000.0
#define X_GAIN_I 10.0
#define X_GAIN_D 200.0

#define Y_GAIN_P 50.0
#define Y_GAIN_I 1.0
#define Y_GAIN_D 2.0

#define YAW_T 0.004 // Loop time.
#define DEPTH_T 0.1 // Loop time.

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

struct Quaternion
{
    double w, x, y, z;
};

struct EulerAngles
{
    double roll, pitch, yaw;
};

EulerAngles ToEulerAngles(Quaternion q)
{
    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}

float target_yaw = 0.0;
float target_x = 0.0;
float target_y = 0.0;
float target_depth = 55.0;
float velocity = 250.0;
void msgCallbackTarget(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    target_yaw = msg->data[0];
    target_depth = msg->data[1];
    velocity = msg->data[2];
    target_x = msg->data[3];
    target_y = msg->data[4];
}

int move_command = 0;
uint8_t yaw_control = 0;
uint8_t depth_control = 0;
uint8_t pos_xy_control = 0;
uint8_t stop_truster_flag = 0;
uint8_t yaw_init_flag = 0;
void msgCallbackCommand(const std_msgs::UInt8::ConstPtr &msg)
{
    uint8_t input;

    input = msg->data;

    if (input == 1)
    {
        move_command = 1;
        pos_xy_control = 0;
    }
    else if (input == 2)
    {
        move_command = 2;
        pos_xy_control = 0;
    }
    else if (input == 3)
    {
        move_command = 3;
        pos_xy_control = 0;
    }
    else if (input == 4)
    {
        move_command = 4;
        pos_xy_control = 0;
    }
    else if (input == 5)
    {
        move_command = 0;
    }
    else if (input == 6)
    {
        yaw_control = 1;
    }
    else if (input == 7)
    {
        yaw_control = 0;
    }
    else if (input == 8)
    {
        depth_control = 1;
    }
    else if (input == 9)
    {
        depth_control = 1;
    }
    else if (input == 10)
    {
        stop_truster_flag = 1;
    }
    else if (input == 11)
    {
        yaw_init_flag = 1;
    }
    
    else if (input == 12) 
    {
        move_command = 1;
    	pos_xy_control = 1; // move pos_x pid control 
    }
    else if (input == 13) 
    {
    	move_command = 2;
    	pos_xy_control = 1; // move pos_x pid control 
    }
    else if (input == 14) 
    {
    	move_command = 3;
    	pos_xy_control = 1; // move pos_y pid control 
    }
    else if (input == 15) 
    {
    	move_command = 4;
    	pos_xy_control = 1; // move pos_y pid control 
    }
}

ros::Publisher *pub_thruster0_ptr; //- 상승, 오른 히브
ros::Publisher *pub_thruster1_ptr; //- 상승, 왼 히브

ros::Publisher *pub_thruster2_ptr; //- 전진, 오른
ros::Publisher *pub_thruster3_ptr; //- 전진, 왼

ros::Publisher *pub_thruster4_ptr; // 오른 뒤, - 반시계
ros::Publisher *pub_thruster5_ptr; // 오른 앞, - 시계
ros::Publisher *pub_thruster6_ptr; // 왼 앞, - 반시계
ros::Publisher *pub_thruster7_ptr; // 왼 뒤, - 시계

double error_pid_depth, error_pid_depth1;
double P_angle_pid_depth;
double P_depth, I_depth, D_depth, PID_depth;

float current_depth = 0.0;
void msgCallbackDepth(const sensor_msgs::FluidPressure::ConstPtr &msg)
{
    double thruster_0_output = 0.0;
    double thruster_1_output = 0.0;

    uuv_gazebo_ros_plugins_msgs::FloatStamped thruster_msg;

    current_depth = ((msg->fluid_pressure / 101.325) - 1) * 10;

    /////////////////////////depth control
    error_pid_depth = target_depth - current_depth;
    P_depth = error_pid_depth * DEPTH_GAIN_P;                                // Inner P control
    D_depth = (error_pid_depth - error_pid_depth1) / DEPTH_T * DEPTH_GAIN_D; // Inner D control
    I_depth += (error_pid_depth)*DEPTH_T * DEPTH_GAIN_I;                     // Inner I control
    I_depth = constrain(I_depth, -500, 500);                                 // I control must be limited to prevent being jerk.

    PID_depth = P_depth + D_depth + I_depth;

    thruster_0_output = PID_depth;
    thruster_1_output = PID_depth;

    thruster_0_output = constrain(thruster_0_output, -750.0, 750.0);
    thruster_1_output = constrain(thruster_1_output, -750.0, 750.0);

    thruster_msg.data = thruster_0_output;
    pub_thruster0_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_1_output;
    pub_thruster1_ptr->publish(thruster_msg);

    error_pid_depth1 = error_pid_depth;
}

float current_x = 0.0; 
float current_y = 0.0; 

double error_yaw;
double error_pid_yaw, error_pid_yaw1;
double error_x, error_y, error_x1, error_y1;
double P_angle_pid_yaw;
double P_yaw, I_yaw, D_yaw, PID_yaw;
double P_x, P_y, I_x, I_y, D_x, D_y, PID_x, PID_y;

float current_roll = 0.0;
float current_pitch = 0.0;
float current_yaw = 0.0;
float current_acc_yaw = 0.0;

int yaw_calib = 0;
float yaw_calib2 = 0.0;
float yaw_temp = 0.0;
float pre_yaw = 0.0;

void msgCallbackPos(const nav_msgs::Odometry::ConstPtr &msg)
{
    current_x = msg->pose.pose.position.x;
    current_y = msg->pose.pose.position.y;
    ROS_INFO("current x-position is %f", current_x);
    ROS_INFO("current y-position is %f", current_y);
    ROS_INFO("PID_thrust y %f", PID_y);
    ROS_INFO("Pose xy control %d", pos_xy_control);
}

void msgCallbackIMU(const sensor_msgs::Imu::ConstPtr &msg)
{
    double thruster_2_output = 0.0;
    double thruster_3_output = 0.0;
    double thruster_4_output = 0.0;
    double thruster_5_output = 0.0;
    double thruster_6_output = 0.0;
    double thruster_7_output = 0.0;

    uuv_gazebo_ros_plugins_msgs::FloatStamped thruster_msg;

    Quaternion Q_angles;
    EulerAngles E_angles;

    Q_angles.x = msg->orientation.x;
    Q_angles.y = msg->orientation.y;
    Q_angles.z = msg->orientation.z;
    Q_angles.w = msg->orientation.w;

    E_angles = ToEulerAngles(Q_angles);

    current_roll = E_angles.roll;
    current_pitch = E_angles.pitch;
    current_yaw = E_angles.yaw;

    current_acc_yaw = msg->angular_velocity.z;

    // make yaw more good, make yaw init.
    yaw_temp = current_yaw;
    current_yaw -= yaw_calib2;

    if (yaw_init_flag == 1)
    {
        yaw_init_flag = 0;
        yaw_calib2 = yaw_temp;
        pre_yaw = 0;
        current_yaw = 0;
        yaw_calib = 0;
    }

    if (current_yaw - pre_yaw > 4.712388)
        yaw_calib--;
    else if (current_yaw - pre_yaw < -4.712388)
        yaw_calib++;
    pre_yaw = current_yaw;
    current_yaw += 3.141592 * yaw_calib * 2;


    //----yaw control----//

    error_yaw = target_yaw - current_yaw;    // angle def
    P_angle_pid_yaw = YAW_ANGLE_GAIN_P * error_yaw; // angle def + outer P control

    error_pid_yaw = P_angle_pid_yaw - current_acc_yaw; // Pcontrol_angle - angle rate = PID Goal

    P_yaw = error_pid_yaw * YAW_GAIN_P;                            // Inner P control
    D_yaw = (error_pid_yaw - error_pid_yaw1) / YAW_T * YAW_GAIN_D; // Inner D control
    I_yaw += (error_pid_yaw)*YAW_T * YAW_GAIN_I;                   // Inner I control
    I_yaw = constrain(I_yaw, -500.0, 500.0);                       // I control must be limited to prevent being jerk.

    PID_yaw = P_yaw + D_yaw + I_yaw;


    //----xy control----//
    error_x = target_x - current_x;
    error_y = target_y - current_y;
    
    P_x = error_x*X_GAIN_P;
    I_x += (error_x)*X_GAIN_I;
    I_x = constrain(I_x, -750.0, 700.0);
    D_x = (error_x - error_x1) / YAW_T *X_GAIN_D;

    P_y = error_y*Y_GAIN_P;
    I_y += (error_y)*Y_GAIN_I;
    I_y = constrain(I_y, -10.0, 10.0);
    D_y = (error_y - error_y1) / YAW_T *Y_GAIN_D;

    PID_x = P_x + I_x + D_x;
    PID_y = P_y + I_y + D_y;
    
    if (move_command == 0) // stop
    {
    
        thruster_4_output = -PID_yaw - THROTTLE;
        thruster_5_output = +PID_yaw + THROTTLE;
        thruster_6_output = -PID_yaw - THROTTLE;
        thruster_7_output = +PID_yaw + THROTTLE;

        thruster_2_output = 0;
        thruster_3_output = 0;		

    }
    else if (move_command == 1) // forward
    {
        thruster_4_output = -PID_yaw - THROTTLE;
        thruster_5_output = +PID_yaw + THROTTLE;
        thruster_6_output = -PID_yaw - THROTTLE;
        thruster_7_output = +PID_yaw + THROTTLE;
	
	if (pos_xy_control == 0)
	{
            thruster_2_output = -velocity;
            thruster_3_output = -velocity;
        }
	else if (pos_xy_control == 1)
	{
            thruster_2_output = PID_x;
            thruster_3_output = PID_x;
        }
        
    }
    else if (move_command == 2) // backward
    {
        thruster_4_output = -PID_yaw - THROTTLE;
        thruster_5_output = +PID_yaw + THROTTLE;
        thruster_6_output = -PID_yaw - THROTTLE;
        thruster_7_output = +PID_yaw + THROTTLE;

	if (pos_xy_control == 0)
	{
            thruster_2_output = +velocity;
            thruster_3_output = +velocity;
        }
	else if (pos_xy_control == 1)
	{
            thruster_2_output = PID_x;
            thruster_3_output = PID_x;
        }
        
    }
    else if (move_command == 4) // right
    {
        if (pos_xy_control == 0)
        {
		thruster_4_output = -PID_yaw - THROTTLE - velocity;
		thruster_5_output = +PID_yaw + THROTTLE - velocity;
		thruster_6_output = -PID_yaw - THROTTLE + velocity;
		thruster_7_output = +PID_yaw + THROTTLE + velocity;

		thruster_2_output = 0;
		thruster_3_output = 0;        
        }
        
        else if (pos_xy_control == 1)
        {
		thruster_4_output = -PID_yaw - THROTTLE - PID_y;
		thruster_5_output = +PID_yaw + THROTTLE - PID_y;
		thruster_6_output = -PID_yaw - THROTTLE + PID_y;
		thruster_7_output = +PID_yaw + THROTTLE + PID_y;

		thruster_2_output = PID_x;
		thruster_3_output = PID_x;            
        }

    }
    else if (move_command == 3) // left
    {
        if (pos_xy_control == 0)
        {
		thruster_4_output = -PID_yaw - THROTTLE + velocity;
		thruster_5_output = +PID_yaw + THROTTLE + velocity;
		thruster_6_output = -PID_yaw - THROTTLE - velocity;
		thruster_7_output = +PID_yaw + THROTTLE - velocity;

		thruster_2_output = 0;
		thruster_3_output = 0;        
        }
        
        else if (pos_xy_control == 1)
        {
		thruster_4_output = -PID_yaw - THROTTLE - PID_y;
		thruster_5_output = +PID_yaw + THROTTLE - PID_y;
		thruster_6_output = -PID_yaw - THROTTLE + PID_y;
		thruster_7_output = +PID_yaw + THROTTLE + PID_y;

		thruster_2_output = PID_x;
		thruster_3_output = PID_x;            
        }

    }
    
    thruster_2_output = constrain(thruster_2_output, -1500, 1500);
    thruster_3_output = constrain(thruster_3_output, -1500, 1500);
    thruster_4_output = constrain(thruster_4_output, -1500, 1500);
    thruster_5_output = constrain(thruster_5_output, -1500, 1500);
    thruster_6_output = constrain(thruster_6_output, -1500, 1500);
    thruster_7_output = constrain(thruster_7_output, -1500, 1500);

    thruster_msg.data = thruster_2_output;
    pub_thruster2_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_3_output;
    pub_thruster3_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_4_output;
    pub_thruster4_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_5_output;
    pub_thruster5_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_6_output;
    pub_thruster6_ptr->publish(thruster_msg);

    thruster_msg.data = thruster_7_output;
    pub_thruster7_ptr->publish(thruster_msg);

    error_pid_yaw1 = error_pid_yaw;
    error_x1 = error_x;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "auv_controller_node");
    ros::NodeHandle nh;

    ros::Subscriber depth_sub = nh.subscribe("/cyclops/pressure", 100, msgCallbackDepth);
    ros::Subscriber pos_sub = nh.subscribe("/cyclops/pose_gt", 100, msgCallbackPos);
    ros::Subscriber imu_sub = nh.subscribe("/cyclops/imu", 100, msgCallbackIMU);
    ros::Subscriber command_sub = nh.subscribe("/cyclops/command", 100, msgCallbackCommand);
    ros::Subscriber target_sub = nh.subscribe("/cyclops/control_target", 100, msgCallbackTarget);

    ros::Publisher thruster0_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/0/input", 100);
    ros::Publisher thruster1_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/1/input", 100);
    ros::Publisher thruster2_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/2/input", 100);
    ros::Publisher thruster3_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/3/input", 100);
    ros::Publisher thruster4_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/4/input", 100);
    ros::Publisher thruster5_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/5/input", 100);
    ros::Publisher thruster6_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/6/input", 100);
    ros::Publisher thruster7_pub = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/7/input", 100);
    uuv_gazebo_ros_plugins_msgs::FloatStamped thruster_msg;

    pub_thruster0_ptr = &thruster0_pub;
    pub_thruster1_ptr = &thruster1_pub;
    pub_thruster2_ptr = &thruster2_pub;
    pub_thruster3_ptr = &thruster3_pub;
    pub_thruster4_ptr = &thruster4_pub;
    pub_thruster5_ptr = &thruster5_pub;
    pub_thruster6_ptr = &thruster6_pub;
    pub_thruster7_ptr = &thruster7_pub;

    ros::Publisher state_pub = nh.advertise<std_msgs::UInt8MultiArray>("/cyclops/control_state", 100);
    ros::Publisher sensor_pub = nh.advertise<std_msgs::Float32MultiArray>("/cyclops/RPYD", 100);
    std_msgs::UInt8MultiArray state_msg;
    state_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    state_msg.layout.dim[0].label = "control state";
    state_msg.layout.dim[0].size = 4;
    state_msg.layout.dim[0].stride = 4;
    state_msg.layout.data_offset = 4;
    // 1 channel, size 4 (control_state, yaw_control, depth_control, pid_xy_control)
    state_msg.data.resize(4);
    state_msg.data[0] = 0; // control state
    state_msg.data[1] = 0; // yaw control
    state_msg.data[2] = 0; // depth control
    state_msg.data[3] = 0; // pos-xy control  

    std_msgs::Float32MultiArray sensor_msg;
    sensor_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    sensor_msg.layout.dim[0].label = "current sensor values";
    sensor_msg.layout.dim[0].size = 4;
    sensor_msg.layout.dim[0].stride = 4;
    sensor_msg.layout.data_offset = 4;
    // 1 channel, size 4 (roll, pitch, yaw, depth)
    sensor_msg.data.resize(4);
    sensor_msg.data[0] = 0.0; // current roll
    sensor_msg.data[1] = 0.0; // current pithch
    sensor_msg.data[2] = 0.0; // current yaw
    sensor_msg.data[3] = 0.0; // current depth

    ros::Rate loop_rate(25);

    while (ros::ok())
    {
        state_msg.data[0] = move_command;  // control state
        state_msg.data[1] = yaw_control;   // yaw control
        state_msg.data[2] = depth_control; // depth control
        state_msg.data[3] = pos_xy_control;
        state_pub.publish(state_msg);

        sensor_msg.data[0] = current_roll;  // current roll
        sensor_msg.data[1] = current_pitch; // current pithch
        sensor_msg.data[2] = current_yaw;   // current yaw
        sensor_msg.data[3] = current_depth; // current depth
        sensor_pub.publish(sensor_msg);

        ros::spinOnce();
        loop_rate.sleep();        
    }

    return 0;
}
