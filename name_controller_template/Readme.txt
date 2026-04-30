Programming Assignment #2 — <YourStudentID>_<YourName>
==============================================================

Package name : <yourname>_controller
Author       : <YourName>  <your_email@postech.ac.kr>

----------------------------------------------------------------
1. Build
----------------------------------------------------------------
    cd ~/uuv_ws
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
    # Yaw 90° step:
    #   - Press 'y' to enable yaw control
    #   - Press 'i' until target yaw = pi/2
    #   - Save plot as yaw_control_result.jpg

    # Depth 1 m step:
    #   - Press 'p' to enable depth control
    #   - Press 'o' (or 'l') ten times to step target depth by 1 m
    #   - Save plot as depth_control_result.jpg

(Replace <yourname> / <YourName> / <YourStudentID> with your real values.)
