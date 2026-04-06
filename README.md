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
    ros-noetic-desktop-full \
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
| `src/ex_robot1_description/urdf/ex_robot1.urdf` | The URDF you must complete (TODO 1 ~ TODO 6). |
| `src/ex_robot1_description/launch/ex_robot1_rviz.launch` | RViz visualization (used after TODO 1 + 2). |
| `src/ex_robot1_gazebo/world/ex_robot1.world` | Gazebo world (no edits needed). |
| `src/ex_robot1_gazebo/launch/ex_robot1_world.launch` | Spawns the URDF into Gazebo (no edits needed). |

> The `ex_robot1_control` package is **not** included on purpose — you
> will create it yourself in Section 4.5 of the slides.

---

## 4. Workflow

The Lab 4 slides drive everything; the TODO blocks in the URDF are
numbered to match the slide sections.

### 4.1 Pre-class TODOs (Sections 4.2 ~ 4.4)

| TODO | Slide | What to add |
|------|-------|-------------|
| **TODO 1** | p8 (left) | `world` link + fixed joint that places `base_link` on the world |
| **TODO 2** | p8 (right) | `joint1` (continuous) + `link1` (tall box, with **inertial**) |
| **TODO 3** | p24 | `<gazebo>` block with the `gazebo_ros_control` plugin |
| **TODO 4** | p25 | `transmission tran1` for `joint1` |

After **TODO 1 + 2**, check your URDF in RViz:

```bash
roslaunch ex_robot1_description ex_robot1_rviz.launch
```

> When RViz opens, change **Global Options -> Fixed Frame** from
> `base_link` to **`world`** (the URDF root link is `world`, otherwise
> you will see a transform error).

After **TODO 3 + 4**, spawn the robot in Gazebo:

```bash
roslaunch ex_robot1_gazebo ex_robot1_world.launch
```

You should see the manipulator drop under gravity (no controller yet).

### 4.2 Build your own `ex_robot1_control` package (Section 4.5, slide p33)

```bash
cd ~/sim_ws/src
catkin_create_pkg ex_robot1_control \
    controller_manager joint_state_controller \
    effort_controllers robot_state_publisher
```

Then create the two files:

* `ex_robot1_control/config/ex_robot1_control.yaml`
  — `joint_state_controller` + `joint1_position_controller`
* `ex_robot1_control/launch/ex_robot1_control.launch`
  — loads the YAML, spawns the controllers under `/ex_robot1`,
  starts `robot_state_publisher`

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

### 4.3 In-Class Assignment (slide p37) — 2-DoF extension

| TODO | What to add |
|------|-------------|
| **TODO 5** | `joint2` + `link2` in `urdf/ex_robot1.urdf` |
| **TODO 6** | `transmission tran2` for `joint2` |
| - | Add `joint2_position_controller` to `ex_robot1_control.yaml` |
| - | Add `joint2_position_controller` to the spawner args of `ex_robot1_control.launch` |

Verify:

```bash
rostopic pub -1 /ex_robot1/joint1_position_controller/command std_msgs/Float64 'data: 0.5'
rostopic pub -1 /ex_robot1/joint2_position_controller/command std_msgs/Float64 'data: 0.3'
```

Both joints should hold their commanded positions.

Plot the response with `rqt_gui`:

```bash
rosrun rqt_gui rqt_gui
```

Add the `MatPlot` and `Message Publisher` plugins, set the topic to
`/ex_robot1/joint1_position_controller/command`, type `expression =
sin(i/100)`, frequency `200 Hz`, and watch the joint follow the sinusoid.

---

## 5. Hints

### Solid box inertia

For a solid box of dimensions `(a, b, c)` and mass `m` (rotation axes
through the center of mass):

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
inertial block. If you forget the inertial of `base_link`, the spawned
model has no `joint1` and you will see:

```
This robot has a joint named "joint1" which is not in the gazebo model.
```

`base_link` is included as a complete example in `ex_robot1.urdf` —
use it as a template for `link1` (and later `link2`).

### Continuous joint position values

`joint1` and `joint2` are `continuous` joints, so the reported position
is **not wrapped** to `[-pi, pi]`. After a transient (for example a free
fall right after `spawn`), you may see a position like `5.21` rad which
is just `-1.07 + 2*pi`. The PID is working correctly; the absolute value
just includes accumulated rotations.

---

## 6. Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `effort_controllers/JointPositionController` not found | `ros-noetic-effort-controllers` missing | `sudo apt-get install ros-noetic-effort-controllers` |
| `joint1 which is not in the gazebo model` | `base_link` (or any parent link) has no `<inertial>` | Add an inertial block |
| RViz transform error after `display.launch` | Fixed Frame is `base_link`, but URDF root is `world` | Change Fixed Frame to `world` |
| `controller_spawner` keeps retrying | YAML namespace mismatch | The YAML root must be `ex_robot1:` and the spawner `ns="/ex_robot1"` |
| Robot falls over and never stops | `ex_robot1_control.launch` not started | Run both `world.launch` and `control.launch` |

---

## 7. Reference

* Lab 4 slides: `2026_Spring_이동로봇공학_Lab04_Dynamic_simulation_Robot_control.pdf`
* ROS wiki: <http://wiki.ros.org/urdf/Tutorials>
* Gazebo + ros_control: <http://gazebosim.org/tutorials/?tut=ros_control>
