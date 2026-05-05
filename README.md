# Mobile Robotics - Lab Reference Materials

Reference materials from Lab 4, Lab 6, and Lab 7 of **CITE700X Mobile
Robotics (2026 Spring)**, provided to help with **Assignment 2**.

## Download

```bash
git clone https://github.com/HERO-Lab-POSTECH/mobile_robotics.git
cd mobile_robotics
```

That is the only `git` command you need. Everything is inside.

## What is in here

| Folder    | Lab        | Topic                                            |
|-----------|------------|--------------------------------------------------|
| `lab04/`  | Lab 4 (+5) | 1-DoF arm in Gazebo with `ros_control`. Lab 5 reused these same `ex_robot1` packages for PID-control practice. |
| `lab06/`  | Lab 6      | UUV (Cyclops) simulation in Gazebo with depth, IMU, and camera sensors. |
| `lab07/`  | Lab 7      | Depth PID controller for the Cyclops AUV.        |

## Which folder helps with Assignment 2?

**Start with `lab07/`.** Assignment 2 asks you to write a controller, and
`lab07/src/lab_07_pid_controller/` is the closest worked example: it
shows how to read sensor topics, run a fixed-rate control loop, and
publish thruster commands. `lab06/` is useful background if you need to
understand the simulator and topic names; `lab04/` is the simplest
illustration of the ROS control loop pattern.

## Build & run any lab

Each lab is an independent catkin workspace. Pick one and:

```bash
cd lab07          # or lab06, lab04
./install_deps.sh # only present for lab06 and lab07
catkin_make
source devel/setup.bash
```

After that, see the per-lab `README.md` for the exact `roslaunch`
commands.

## Environment

- ROS Noetic on Ubuntu 20.04.
- Gazebo 11 (installed with `ros-noetic-desktop-full`).
- For Lab 6 and Lab 7, `install_deps.sh` adds the UUV simulator
  dependencies via `apt`.

## Per-lab quick reference

- `lab04/README.md` - 1-DoF Gazebo + control launch commands.
- `lab06/README.md` - Cyclops simulation launch commands and topic list.
- `lab07/README.md` - depth PID launch commands and parameter notes.

## Note on the original in-class skeletons

This `main` branch contains the **completed solutions**, intended as
reference. The in-class starter code (with TODOs) was distributed during
the course and lives on the `lab04`, `lab06` branches of this repo.
Assignment 2's own template is on the `assignment2` branch.
