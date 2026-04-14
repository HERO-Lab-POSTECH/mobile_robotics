# Lab 4 - Dynamic Simulation and Robot Control (Complete Solution)

CITE700X / NUCE718H Mobile Robotics, 2026 Spring (Prof. Son-Cheol Yu, POSTECH)

This repository contains the **complete solution** for Lab 4, including
all pre-class TODOs and the in-class assignment (2-DoF extension).

---

## 1. Prerequisites

Install ROS Noetic + the Lab 4 dependencies:

```bash
sudo apt-get update
sudo apt-get install \
    ros-noetic-desktop-full \
    ros-noetic-ros-control \
    ros-noetic-ros-controllers \
    ros-noetic-effort-controllers \
    ros-noetic-joint-state-controller \
    ros-noetic-controller-manager \
    ros-noetic-gazebo-ros-control \
    ros-noetic-urdf-tutorial
```

---

## 2. Clone and build

```bash
cd ~
git clone https://github.com/HERO-Lab-POSTECH/mobile_robotics.git sim_ws
cd ~/sim_ws
catkin_make
source devel/setup.bash
```

Add the `source` command to `~/.bashrc`:

```bash
echo "source ~/sim_ws/devel/setup.bash" >> ~/.bashrc
```

Verify the packages:

```bash
rospack list | grep ex_robot1
# ex_robot1_control      /home/<you>/sim_ws/src/ex_robot1_control
# ex_robot1_description  /home/<you>/sim_ws/src/ex_robot1_description
# ex_robot1_gazebo       /home/<you>/sim_ws/src/ex_robot1_gazebo
```

---

## 3. Packages

| Package | Purpose |
|---------|---------|
| `ex_robot1_description` | URDF model (2-DoF manipulator) + RViz launch |
| `ex_robot1_gazebo` | Gazebo world file + spawn launch |
| `ex_robot1_control` | ros_control YAML config + controller launch |

### File structure

```
src/
├── ex_robot1_description/
│   ├── urdf/ex_robot1.urdf              # Complete 2-DoF URDF
│   └── launch/ex_robot1_rviz.launch     # RViz + joint_state_publisher_gui
├── ex_robot1_gazebo/
│   ├── world/ex_robot1.world            # Ground plane + sun + camera
│   └── launch/ex_robot1_world.launch    # Gazebo spawn
└── ex_robot1_control/
    ├── config/ex_robot1_control.yaml    # PID controllers (joint1 + joint2)
    └── launch/ex_robot1_control.launch  # Controller spawner + robot_state_publisher
```

---

## 4. Run

### 4.1 RViz visualization

```bash
roslaunch ex_robot1_description ex_robot1_rviz.launch
```

> Set **Global Options -> Fixed Frame** to `world` in RViz.

Use the GUI sliders to rotate joint1 and joint2.

### 4.2 Gazebo dynamic simulation

```bash
# Terminal 1
roslaunch ex_robot1_gazebo ex_robot1_world.launch

# Terminal 2
roslaunch ex_robot1_control ex_robot1_control.launch
```

### 4.3 Send position commands

```bash
rostopic pub -1 /ex_robot1/joint1_position_controller/command std_msgs/Float64 'data: 0.5'
rostopic pub -1 /ex_robot1/joint2_position_controller/command std_msgs/Float64 'data: 0.3'
```

Both joints should hold their commanded positions.

### 4.4 Sine tracking with rqt_gui (slide p35)

```bash
rosrun rqt_gui rqt_gui
```

* **MatPlot** plugin: subscribe to `/ex_robot1/joint1_position_controller/state/process_value`
* **Message Publisher** plugin: topic `/ex_robot1/joint1_position_controller/command`,
  expression `sin(i/100)`, frequency `200 Hz`

---

## 5. Robot specifications

| Link | Size (m) | Mass (kg) | Parent joint | Joint axis | Joint type |
|------|----------|-----------|-------------|------------|------------|
| base_link | 1.0 x 1.0 x 1.0 | 12 | fixed (world) | - | fixed |
| link1 | 0.3 x 0.3 x 3.0 | 3 | joint1 | y-axis | continuous |
| link2 | 0.3 x 0.3 x 3.0 | 3 | joint2 | y-axis | continuous |

### Solid box inertia

For a solid box of dimensions `(a, b, c)` and mass `m`:

```
ixx = (1/12) * m * (b^2 + c^2)
iyy = (1/12) * m * (a^2 + c^2)
izz = (1/12) * m * (a^2 + b^2)
```

| Link | ixx | iyy | izz |
|------|-----|-----|-----|
| base_link (1x1x1, 12 kg) | 2.0 | 2.0 | 2.0 |
| link1/link2 (0.3x0.3x3, 3 kg) | 2.2725 | 2.2725 | 0.045 |

### Controller configuration

Both joints use `effort_controllers/JointPositionController`:

```yaml
pid: {p: 200.0, i: 0.01, d: 50.0}
```

---

## 6. Slide-to-code mapping

The URDF section markers (TODO 1~6) are kept for reference to the Lab 4 slides:

| Section | Slide | Content |
|---------|-------|---------|
| TODO 1 | p8 (left) | `world` link + fixed joint |
| TODO 2 | p8 (right) | `joint1` + `link1` (with inertial) |
| TODO 3 | p24 | `gazebo_ros_control` plugin |
| TODO 4 | p25 | Transmission `tran1` for `joint1` |
| TODO 5 | p37 | `joint2` + `link2` (in-class, 2-DoF extension) |
| TODO 6 | p37 | Transmission `tran2` for `joint2` |

---

## 7. Diagnostics

```bash
rosservice call /ex_robot1/controller_manager/list_controllers
rostopic echo -n 1 /ex_robot1/joint_states
rostopic list | grep ex_robot1
```

---

## 8. Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `effort_controllers/JointPositionController` not found | `ros-noetic-effort-controllers` missing | `sudo apt-get install ros-noetic-effort-controllers` |
| `joint1 which is not in the gazebo model` | A link is missing `<inertial>` | Add an inertial block to every link with `<visual>` |
| RViz transform error | Fixed Frame is `base_link` | Change Fixed Frame to `world` |
| `controller_spawner` keeps retrying | YAML namespace mismatch | YAML root must be `ex_robot1:`, spawner `ns="/ex_robot1"` |
| Robot falls over and never stops | `control.launch` not started | Run both `world.launch` and `control.launch` |

---

## 9. Reference

* Lab 4 slides: `2026_Spring_이동로봇공학_Lab04_Dynamic_simulation_Robot_control.pdf`
* ROS wiki: <http://wiki.ros.org/urdf/Tutorials>
* Gazebo + ros_control: <http://gazebosim.org/tutorials/?tut=ros_control>
