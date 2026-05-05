# Lab 07 - Cyclops Cascaded Depth and Yaw PID Controller

POSTECH CITE700X Mobile Robotics (2026 Spring).

## Setup

Lab 07 shares the same student distribution branch as Lab 06. If you already
built Lab 06, you only need to build the new package:

```bash
git clone https://github.com/HERO-Lab-POSTECH/mobile_robotics.git
cd mobile_robotics/lab06
sudo ./install_deps.sh
catkin_make
source devel/setup.bash
```

## Run

Four terminals are required:

```bash
# Terminal 1 - Gazebo world
roslaunch uuv_gazebo_worlds heroslam_map.launch

# Terminal 2 - spawn Cyclops
roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch

# Terminal 3 - PID controller node
rosrun lab_07_pid_controller auv_controller_node

# Terminal 4 - keyboard node (keep focus here)
rosrun lab_07_pid_controller teleop_keyboard
```

With correct PID gains, Cyclops holds depth and yaw on `q` (stop) and responds
to motion keys.

## Key Bindings

| Key | Action |
|---|---|
| `w` / `s` | Forward / Backward |
| `a` / `d` | Sway-left / Sway-right |
| `i` / `k` | Heave-up / Heave-down |
| `q` | Stop (depth- and yaw-hold) |
| `z` / `x` | Speed +10 / -10 |

## PID Structure

The controller uses a cascaded loop:

- Depth (heave): single PID loop, error = target depth - current depth.
- Yaw: outer loop converts heading error to desired yaw rate; inner loop converts
  yaw rate error to thruster command.

Gain tuning is an open-ended exercise - there is no single correct answer.
A stable response with a rise time of a few seconds and no sustained oscillation
is acceptable.

## Packages

- `lab_07_pid_controller/` - PID controller and teleop nodes (contains TODOs).
- `uuv_cyclops_description/` - Cyclops URDF with IMU plugin.
- `uuv_gazebo_worlds/` - heroslam world.

## Troubleshooting

- If Cyclops oscillates heavily, reduce the P and D gains by half and re-test.
- If depth does not converge, check that the IMU topic `/cyclops/imu` is
  publishing before starting `auv_controller_node`.
- If `catkin_make` fails with missing headers, re-run `sudo ./install_deps.sh`
  and try again.
