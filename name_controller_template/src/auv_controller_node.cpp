// =============================================================================
// Programming Assignment #2 — auv_controller_node skeleton
//
// You must implement a closed-loop PID controller for the Cyclops AUV that
// satisfies the spec in 2026_CITE700X_Assignment2.pdf:
//
//   - Yaw control: 125 Hz   (driven by /cyclops/imu callback)
//   - Depth control: 10 Hz  (driven by /cyclops/pressure callback)
//   - Yaw step (90°): settle within 2 s, |yaw_error| < 5°
//   - Depth step (1 m): settle within 2 s, |depth_error| < 0.1 m
//
// The skeleton below sets up the topic subscriptions, publishers, and message
// layouts as the spec dictates. EVERY callback body is intentionally empty.
// You are responsible for the PID laws, thruster mixing, and command parsing.
// =============================================================================

#include "ros/ros.h"

#include <std_msgs/UInt8.h>
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/MultiArrayDimension.h>

#include <sensor_msgs/FluidPressure.h>
#include <sensor_msgs/Imu.h>

#include "uuv_gazebo_ros_plugins_msgs/FloatStamped.h"

#include <cmath>

// -----------------------------------------------------------------------------
// Helper: quaternion -> Euler angles (provided so you can focus on control,
// not on the math). DO NOT change unless you know what you are doing.
// -----------------------------------------------------------------------------
struct Quaternion { double w, x, y, z; };
struct EulerAngles { double roll, pitch, yaw; };

EulerAngles ToEulerAngles(Quaternion q)
{
    EulerAngles a;
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    a.roll = std::atan2(sinr_cosp, cosr_cosp);

    double sinp = 2 * (q.w * q.y - q.z * q.x);
    a.pitch = std::abs(sinp) >= 1 ? std::copysign(M_PI / 2, sinp) : std::asin(sinp);

    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    a.yaw = std::atan2(siny_cosp, cosy_cosp);
    return a;
}

// -----------------------------------------------------------------------------
// Global state -- shared between callbacks. You may add gain #defines, error
// trackers, integral accumulators, etc. as needed.
// -----------------------------------------------------------------------------
// Targets (filled by /cyclops/control_target callback)
float target_yaw      = 0.0f;
float target_depth    = 0.0f;
float velocity        = 0.0f;

// Control-mode flags (set by /cyclops/command callback)
uint8_t motion_state    = 0;  // 0 stop, 1 fwd, 2 bwd, 3 sway-L, 4 sway-R
uint8_t yaw_control_on  = 0;
uint8_t depth_control_on= 0;
uint8_t yaw_init_flag   = 0;
uint8_t stop_all_flag   = 0;

// Sensor measurements (filled by IMU + pressure callbacks)
float current_roll  = 0.0f;
float current_pitch = 0.0f;
float current_yaw   = 0.0f;
float current_depth = 0.0f;

// Thruster publisher pointers (assigned in main, used by IMU + depth callbacks)
ros::Publisher *pub_thr[8] = {nullptr};

// =============================================================================
// TODO: add your gain #defines and PID-state variables here.
// =============================================================================

// -----------------------------------------------------------------------------
// Callback: /cyclops/control_target (Float32MultiArray, 3 fields per spec)
// -----------------------------------------------------------------------------
void targetCb(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    // TODO: read target_yaw, target_depth, velocity from msg->data[0..2].
    //       (See Assignment 2 PDF §2.5.)
}

// -----------------------------------------------------------------------------
// Callback: /cyclops/command (UInt8, codes 1..11 per spec §2.4)
// -----------------------------------------------------------------------------
void commandCb(const std_msgs::UInt8::ConstPtr &msg)
{
    // TODO: dispatch the command code into motion_state / yaw_control_on /
    //       depth_control_on / stop_all_flag / yaw_init_flag.
    //       Codes 1..5 set motion_state; 6/7 toggle yaw control;
    //       8/9 toggle depth control; 10 stops all thrusters; 11 zeros yaw.
}

// -----------------------------------------------------------------------------
// Depth control loop (10 Hz, driven by /cyclops/pressure)
// -----------------------------------------------------------------------------
void depthCb(const sensor_msgs::FluidPressure::ConstPtr &msg)
{
    // Depth from absolute pressure (provided so units match the spec).
    current_depth = ((msg->fluid_pressure / 101.325f) - 1.0f) * 10.0f;

    // TODO: implement depth PID law (P + I + D with anti-windup) and publish
    //       the result on /cyclops/thrusters/0/input and /cyclops/thrusters/1/input
    //       (the heave thruster pair). Apply the result only when
    //       depth_control_on == 1.
}

// -----------------------------------------------------------------------------
// Yaw control loop (125 Hz, driven by /cyclops/imu)
// -----------------------------------------------------------------------------
void imuCb(const sensor_msgs::Imu::ConstPtr &msg)
{
    Quaternion q = { msg->orientation.w, msg->orientation.x,
                     msg->orientation.y, msg->orientation.z };
    EulerAngles e = ToEulerAngles(q);
    current_roll  = e.roll;
    current_pitch = e.pitch;
    current_yaw   = e.yaw;
    // (msg->angular_velocity.z is the yaw rate -- you'll likely need it.)

    // TODO 1: handle yaw_init_flag (zero out current_yaw on demand) AND
    //         yaw wrap-around so the error stays continuous across ±pi.
    //
    // TODO 2: implement the cascaded yaw PID law per spec §2.3.
    //         Outer P on angle error -> rate setpoint;
    //         inner PID on (rate setpoint - measured yaw rate).
    //
    // TODO 3: thruster mixing for every motion_state branch
    //         (0 stop, 1 fwd, 2 bwd, 3 sway-L, 4 sway-R).
    //         Translation is an additive offset on the relevant thrusters,
    //         NOT a separate PID loop (spec §2.4).
    //
    // TODO 4: respect stop_all_flag (10) -- when set, override every output to 0.
    //         Respect yaw_control_on (6/7) -- when off, do not apply yaw PID
    //         on thrusters 4..7.
    //
    // TODO 5: publish thruster commands on /cyclops/thrusters/0..7/input.
}

// =============================================================================
int main(int argc, char **argv)
{
    ros::init(argc, argv, "auv_controller_node");
    ros::NodeHandle nh;

    // ---- Subscribers (spec §2.1) ---------------------------------------------
    ros::Subscriber sub_pressure = nh.subscribe("/cyclops/pressure",       100, depthCb);
    ros::Subscriber sub_imu      = nh.subscribe("/cyclops/imu",            100, imuCb);
    ros::Subscriber sub_command  = nh.subscribe("/cyclops/command",        100, commandCb);
    ros::Subscriber sub_target   = nh.subscribe("/cyclops/control_target", 100, targetCb);

    // ---- Thruster publishers (spec §2.2) -------------------------------------
    ros::Publisher pub_thr0 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/0/input", 100);
    ros::Publisher pub_thr1 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/1/input", 100);
    ros::Publisher pub_thr2 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/2/input", 100);
    ros::Publisher pub_thr3 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/3/input", 100);
    ros::Publisher pub_thr4 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/4/input", 100);
    ros::Publisher pub_thr5 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/5/input", 100);
    ros::Publisher pub_thr6 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/6/input", 100);
    ros::Publisher pub_thr7 = nh.advertise<uuv_gazebo_ros_plugins_msgs::FloatStamped>("/cyclops/thrusters/7/input", 100);
    pub_thr[0] = &pub_thr0;  pub_thr[1] = &pub_thr1;
    pub_thr[2] = &pub_thr2;  pub_thr[3] = &pub_thr3;
    pub_thr[4] = &pub_thr4;  pub_thr[5] = &pub_thr5;
    pub_thr[6] = &pub_thr6;  pub_thr[7] = &pub_thr7;

    // ---- /cyclops/control_state (UInt8MultiArray, 3 fields per spec §2.6) -----
    ros::Publisher pub_state = nh.advertise<std_msgs::UInt8MultiArray>("/cyclops/control_state", 100);
    std_msgs::UInt8MultiArray state_msg;
    state_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    state_msg.layout.dim[0].label  = "control_state";
    state_msg.layout.dim[0].size   = 3;
    state_msg.layout.dim[0].stride = 3;
    state_msg.data.resize(3);

    // ---- /cyclops/RPYD (Float32MultiArray, 4 fields per spec §2.7) ------------
    ros::Publisher pub_rpyd = nh.advertise<std_msgs::Float32MultiArray>("/cyclops/RPYD", 100);
    std_msgs::Float32MultiArray rpyd_msg;
    rpyd_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    rpyd_msg.layout.dim[0].label  = "RPYD";
    rpyd_msg.layout.dim[0].size   = 4;
    rpyd_msg.layout.dim[0].stride = 4;
    rpyd_msg.data.resize(4);

    ros::Rate loop_rate(100); // 100 Hz publish loop (spec §3.5)

    while (ros::ok())
    {
        // TODO: fill state_msg.data[0..2] (motion_state, yaw_control_on,
        //                                  depth_control_on) and publish.
        // TODO: fill rpyd_msg.data[0..3] (current_roll, current_pitch,
        //                                 current_yaw, current_depth) and publish.

        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
