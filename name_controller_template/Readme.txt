Programming Assignment #2 — <YourStudentID>_<YourName>
==============================================================

Package name : <yourname>_controller
Author       : <YourName>  <your_email@postech.ac.kr>

----------------------------------------------------------------
1. Build
----------------------------------------------------------------
    # Use the Lab 7 workspace from the main distribution:
    cd ~/mobile_robotics/lab07
    catkin_make
    source devel/setup.bash

----------------------------------------------------------------
2. Launch the simulator (re-uses Lab 07 setup)
----------------------------------------------------------------
    # Terminal 1 — World
    roslaunch uuv_gazebo_worlds heroslam_map.launch

    # Terminal 2 — Spawn Cyclops
    roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch

----------------------------------------------------------------
3. Run your nodes
----------------------------------------------------------------
    # Terminal 3 — controller
    rosrun <yourname>_controller auv_controller_node

    # Terminal 4 — teleop (keyboard focus must stay here)
    rosrun <yourname>_controller teleop_keyboard

----------------------------------------------------------------
4. Reproduce the step-response plots
----------------------------------------------------------------
    Option A - via teleop (after teleop_keyboard is implemented):
        # Yaw 90 deg step:
        #   - Press 'y' to enable yaw control
        #   - Press 'i' until target yaw = pi/2
        # Depth 1 m step:
        #   - Press 'p' to enable depth control
        #   - Press 'o' ten times to step target depth by 1 m

    Option B - direct topic publish (works with controller node alone):
        # Watch the response live:
        rqt_plot /cyclops/RPYD/data[2] /cyclops/RPYD/data[3]

        # Yaw 90 deg step:
        rostopic pub -1 /cyclops/command std_msgs/UInt8 "data: 6"
        rostopic pub -1 /cyclops/control_target std_msgs/Float32MultiArray \
          "data: [1.5708, 0.0, 0.0]"

        # Depth 1 m step:
        rostopic pub -1 /cyclops/command std_msgs/UInt8 "data: 8"
        rostopic pub -1 /cyclops/control_target std_msgs/Float32MultiArray \
          "data: [0.0, 1.0, 0.0]"

    Save plots as yaw_control_result.jpg and depth_control_result.jpg
    inside your package folder. See README.md section 4.4 for the
    rosbag + matplotlib recipe.

(Replace <yourname> / <YourName> / <YourStudentID> with your real values.)
