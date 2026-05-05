# Programming Assignment #2

**Student ID:** `<YourStudentID>`
**Name:** `<YourName>`
**Email:** `<your_email@postech.ac.kr>`
**Package name:** `<yourname>_controller`

> Replace every `<...>` placeholder above with your real values before submitting.

---

## 1. Build

Use the Lab 7 workspace from the `main` distribution:

```bash
cd ~/mobile_robotics/lab07
catkin_make
source devel/setup.bash
```

## 2. Launch the simulator

Re-uses the Lab 7 setup; run each command in a separate terminal.

```bash
# Terminal 1 - World
roslaunch uuv_gazebo_worlds heroslam_map.launch

# Terminal 2 - Spawn Cyclops
roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch
```

## 3. Run the nodes

```bash
# Terminal 3 - controller
rosrun <yourname>_controller auv_controller_node

# Terminal 4 - teleop (keyboard focus must stay here)
rosrun <yourname>_controller teleop_keyboard
```

## 4. Reproduce the step-response plots

### Option A - via teleop

After `teleop_keyboard` is implemented:

- **Yaw 90 deg step**
  1. Press `y` to enable yaw control.
  2. Press `i` until the target yaw reaches `pi/2`.
- **Depth 1 m step**
  1. Press `p` to enable depth control.
  2. Press `o` ten times to step the target depth by 1 m.

### Option B - direct topic publish

Works with the controller node alone (teleop not required):

```bash
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
```

Save plots as `yaw_control_result.jpg` and `depth_control_result.jpg` inside your package folder. See the assignment PDF section 4.10 for the `rosbag` + matplotlib recipe.

---

## 5. Notes (optional)

Use this section for anything specific to your implementation that the grader should know: chosen language (C++ / Python), final PID gains, deviations from the default workflow, etc. Delete this section if you have nothing to add.
