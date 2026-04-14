# Lab 4 - Dynamic Simulation and Robot Control

CITE700X / NUCE718H Mobile Robotics, 2026 Spring (Prof. Son-Cheol Yu, POSTECH)

This repository is a ready-to-build catkin workspace for **Lab 4**.
Clone it once and you have a complete `sim_ws` for the lab.

---

## 1. Prerequisites

Install ROS Noetic + the Lab 4 dependencies:

```bash
sudo apt-get update
sudo apt-get install \
    ros-noetic-ros-control \
    ros-noetic-ros-controllers \
    ros-noetic-effort-controllers \
    ros-noetic-joint-state-controller \
    ros-noetic-controller-manager \
    ros-noetic-gazebo-ros-control \
    ros-noetic-urdf-tutorial
```

> `effort_controllers` and `urdf_tutorial` are easy to forget — the
> first one is required by `joint1_position_controller`, the second one
> by the RViz `display.launch` used in Section 4.3 of the slides.

---

## 2. Clone and build

The repository **is** a catkin workspace. Clone it as `sim_ws` and you
are ready to go:

```bash
cd ~
git clone https://github.com/HERO-Lab-POSTECH/mobile_robotics.git sim_ws
cd ~/sim_ws
catkin_make
source devel/setup.bash
```

Add the `source` command to your `~/.bashrc` so every new terminal
picks up the workspace automatically:

```bash
echo "source ~/sim_ws/devel/setup.bash" >> ~/.bashrc
```

After this you should be able to find both packages:

```bash
rospack list | grep ex_robot1
# ex_robot1_description /home/<you>/sim_ws/src/ex_robot1_description
# ex_robot1_gazebo      /home/<you>/sim_ws/src/ex_robot1_gazebo
```

---

## 3. What is in this workspace

| Path | Purpose |
|------|---------|
| `src/ex_robot1_description/urdf/ex_robot1.urdf` | The URDF (TODO 1-4 completed, 1-DoF). |
| `src/ex_robot1_description/launch/ex_robot1_rviz.launch` | RViz visualization. |
| `src/ex_robot1_gazebo/world/ex_robot1.world` | Gazebo world (no edits needed). |
| `src/ex_robot1_gazebo/launch/ex_robot1_world.launch` | Spawns the URDF into Gazebo (no edits needed). |

> The `ex_robot1_control` package is **not** included on purpose — you
> will create it yourself in Section 4.5 of the slides.

---

## 4. Workflow

The Lab 4 slides drive everything.

### 4.1 RViz visualization

```bash
roslaunch ex_robot1_description ex_robot1_rviz.launch
```

> When RViz opens, change **Global Options -> Fixed Frame** from
> `base_link` to **`world`**.

### 4.2 Gazebo dynamic simulation

After adding the gazebo plugin and transmission to the URDF:

```bash
roslaunch ex_robot1_gazebo ex_robot1_world.launch
```

You should see the manipulator drop under gravity (no controller yet).

### 4.3 Build your own `ex_robot1_control` package (slide p33)

```bash
cd ~/sim_ws/src
catkin_create_pkg ex_robot1_control \
    controller_manager joint_state_controller \
    effort_controllers robot_state_publisher
```

Then create the two files:

* `ex_robot1_control/config/ex_robot1_control.yaml`
* `ex_robot1_control/launch/ex_robot1_control.launch`

Build and run, in two terminals:

```bash
cd ~/sim_ws && catkin_make && source devel/setup.bash

# Terminal A
roslaunch ex_robot1_gazebo ex_robot1_world.launch

# Terminal B
roslaunch ex_robot1_control ex_robot1_control.launch
```

Send a position command:

```bash
rostopic pub -1 /ex_robot1/joint1_position_controller/command \
    std_msgs/Float64 'data: 0.5'
```

`link1` should swing to about 0.5 rad and hold there thanks to the PID.

### 4.4 In-Class Assignment — 2-DoF extension

Extend the manipulator to 2-DoF:

1. Add `joint2` + `link2` in `urdf/ex_robot1.urdf`
2. Add `transmission tran2` for `joint2`
3. Add `joint2_position_controller` to `ex_robot1_control.yaml`
4. Add `joint2_position_controller` to the spawner args of `ex_robot1_control.launch`

Verify:

```bash
rostopic pub -1 /ex_robot1/joint1_position_controller/command std_msgs/Float64 'data: 0.5'
rostopic pub -1 /ex_robot1/joint2_position_controller/command std_msgs/Float64 'data: 0.3'
```

### 4.5 rqt_gui monitoring (slide p35)

```bash
rosrun rqt_gui rqt_gui
```

* **MatPlot** plugin: subscribe to `/ex_robot1/joint1_position_controller/state/process_value`
* **Message Publisher** plugin: topic `/ex_robot1/joint1_position_controller/command`,
  expression `sin(i/100)`, frequency `200 Hz`

---

## 5. Hints

### Solid box inertia

For a solid box of dimensions `(a, b, c)` and mass `m`:

```
ixx = (1/12) * m * (b^2 + c^2)
iyy = (1/12) * m * (a^2 + c^2)
izz = (1/12) * m * (a^2 + b^2)
```

For `link1` with `size="0.3 0.3 3"` and `mass="3"`:

```
ixx = iyy = (1/12) * 3 * (0.3^2 + 3.0^2) = 2.2725
izz       = (1/12) * 3 * (0.3^2 + 0.3^2) = 0.045
```

### Why `base_link` needs `<inertial>`

Gazebo silently **removes** any link that has visual / collision but no
inertial block.

---

## 6. Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `effort_controllers/JointPositionController` not found | `ros-noetic-effort-controllers` missing | `sudo apt-get install ros-noetic-effort-controllers` |
| `joint1 which is not in the gazebo model` | A link is missing `<inertial>` | Add an inertial block |
| RViz transform error | Fixed Frame is `base_link` | Change Fixed Frame to `world` |
| `controller_spawner` keeps retrying | YAML namespace mismatch | YAML root must be `ex_robot1:`, spawner `ns="/ex_robot1"` |
| Robot falls over | `control.launch` not started | Run both `world.launch` and `control.launch` |

---

## 7. Reference

* Lab 4 slides: `2026_Spring_이동로봇공학_Lab04_Dynamic_simulation_Robot_control.pdf`
* ROS wiki: <http://wiki.ros.org/urdf/Tutorials>
* Gazebo + ros_control: <http://gazebosim.org/tutorials/?tut=ros_control>
