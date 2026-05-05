# Lab 06 - Cyclops Keyboard Teleop

POSTECH CITE700X Mobile Robotics (2026 Spring).

## Setup

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

# Terminal 3 - control node
rosrun lab_06_package control_node

# Terminal 4 - keyboard node (keep focus here)
rosrun lab_06_package command_node
```

## Key Bindings

| Key | Action |
|---|---|
| `w` / `s` | Forward / Backward |
| `a` / `d` | Sway-left / Sway-right |
| `i` / `k` | Heave-up / Heave-down |
| `q` | Stop |
| `z` / `x` | Speed +10 / -10 |

## Packages

- `lab_06_package/` - keyboard command and control nodes (contains TODOs).
- `uuv_cyclops_description/` - Cyclops URDF.
- `uuv_gazebo_worlds/` - heroslam world.

## Troubleshooting

- If Gazebo hangs on startup, wait 30-60 seconds for the first launch to download models.
- If `command_node` does not respond to keys, make sure Terminal 4 has keyboard focus.
- If `catkin_make` fails with missing headers, re-run `sudo ./install_deps.sh` and try again.
