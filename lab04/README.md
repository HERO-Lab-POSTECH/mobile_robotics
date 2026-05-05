# Lab 4 - 1-DoF Arm in Gazebo with ros_control

Completed solution for Lab 4 (and used as the code base for Lab 5's PID
control practice).

## Build

```bash
cd lab04
catkin_make
source devel/setup.bash
```

## Run

Visualize the arm in RViz:

```bash
roslaunch ex_robot1_description ex_robot1_rviz.launch
# In RViz: Global Options -> Fixed Frame -> world
```

Spawn in Gazebo with the position controller:

```bash
roslaunch ex_robot1_gazebo ex_robot1_world.launch
```

Send a joint command:

```bash
rostopic pub -1 /ex_robot1/joint1_position_controller/command \
  std_msgs/Float64 "data: 1.0"
```

## Packages

- `ex_robot1_description/` - URDF, RViz launch.
- `ex_robot1_gazebo/` - Gazebo world and spawn launch.
- `ex_robot1_control/` - `ros_control` configuration and controller spawner launch.
