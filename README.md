# Lab 07 — Reference Solution (instructor key)

POSTECH CITE700X Mobile Robotics (2026 Spring).
Branch: **`lab07_solution`** of `HERO-Lab-POSTECH/mobile_robotics`.

> **Stop. Read this first.**
>
> This branch contains the **complete reference solution** for the
> Lab 07 in-class assignment (Cyclops cascaded depth + yaw PID
> controller). It is **not** the workspace you should clone before the
> lab.
>
> Use the **`lab06`** branch to work the in-class assignment yourself
> first (Lab 06 + Lab 07 share the same student distribution branch).
> Only consult this branch *after* you have attempted the assignment,
> to compare your implementation or to recover from a blocked spot.

---

## 1. What's in this branch

```
lab07_solution/
├── install_deps.sh
├── README.md                                     # this file
└── src/
    ├── lab_07_pid_controller/                    # FULL implementation (all TODOs filled)
    │   ├── msg/MsgTutorial.msg                   # legacy custom message kept from instructor source
    │   └── src/
    │       ├── auv_controller_node.cpp           # tuned PID gains, full depth + yaw + thruster mixing
    │       └── teleop_keyboard.cpp               # all key bindings (motion + speed + x/y position-hold)
    ├── uuv_cyclops_description/                  # Cyclops URDF (IMU plugin pre-applied for Lab 07)
    ├── uuv_gazebo_worlds/                        # heroslam world
    └── (10 other UUV simulator packages)         # unchanged from lab06 / lab06_solution branches
```

This branch deliberately **does not** include `lab_06_package` — Lab 07
has its own `teleop_keyboard` node and the depth + yaw controller does
not depend on the Lab 06 keyboard publisher. If you also want to drive
Cyclops with the Lab 06 keyboard scheme, use the `lab06` (student) or
`lab06_solution` (reference) branch separately.

## 2. How to use this branch (after attempting the lab)

### Option A — quick comparison (do not overwrite your work)

```bash
git clone -b lab07_solution https://github.com/HERO-Lab-POSTECH/mobile_robotics.git uuv_ws_lab07_sol
diff -ru ~/uuv_ws/src/lab_07_pid_controller/src ~/uuv_ws_lab07_sol/src/lab_07_pid_controller/src
```

### Option B — fresh-build the solution (separate workspace)

```bash
cd ~
git clone -b lab07_solution https://github.com/HERO-Lab-POSTECH/mobile_robotics.git uuv_ws_lab07_sol
cd ~/uuv_ws_lab07_sol
sudo ./install_deps.sh        # safe to skip if already done for Lab 06 / Lab 07
catkin_make
source devel/setup.bash
```

Then run the same four-terminal sequence as in the Lab 07 student
README:

```bash
# Terminal 1
roslaunch uuv_gazebo_worlds heroslam_map.launch
# Terminal 2
roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch
# Terminal 3
rosrun lab_07_pid_controller auv_controller_node
# Terminal 4 (keyboard focus)
rosrun lab_07_pid_controller teleop_keyboard
```

With the tuned gains, Cyclops should hold depth and yaw on `q` (stop)
and respond crisply to `w / s / a / d` (forward / backward / sway-left
/ sway-right) and `i / k` (heave-up / heave-down).

## 3. Differences vs. the `lab06` student branch (Lab 07 portion)

| File | `lab06` student branch | `lab07_solution` (this branch) |
|---|---|---|
| `src/lab_07_pid_controller/src/auv_controller_node.cpp` | TODO 1 (gains stubbed at 1.0), TODO 2a (depth PID law empty), TODO 2b (yaw PID law empty), TODO 3 (only `stop` branch implemented) | All 4 TODOs filled: tuned PID gains, full depth PID law, full cascaded yaw PID law, all motion-state thruster mixing branches (forward / backward / sway-left / sway-right / heave-up / heave-down / stop) |
| `src/lab_07_pid_controller/src/teleop_keyboard.cpp` | full (no TODO) | identical |
| `src/uuv_cyclops_description/urdf/cyclops_sensors.xacro` | IMU swap pre-applied (`libgazebo_ros_imu_sensor.so`, 125 Hz, `/cyclops/imu`) | identical |
| Everything else | identical | identical |

The Cyclops URDF, the world file, and every UUV simulator package are
unchanged across the three branches.

## 4. Tuned PID gains (reference values)

These are the gains used in the instructor demo. They are not the only
working set, but they are stable for the heroslam world.

| Loop | P | I | D |
|---|---:|---:|---:|
| Depth (heave) | 5000 | 0 | 1000 |
| Yaw outer (heading → desired yaw rate) | 5 | 0 | 1 |
| Yaw inner (yaw rate → thruster) | 150 | 0 | 30 |

If your own gains differ but the response is acceptable (rise time a
few seconds, no sustained oscillation, bounded steady-state error),
that's a valid answer — gain tuning is not a single-correct-answer
exercise.

## 5. Key bindings (same as student branch)

| Key | Action |
|---|---|
| `w` / `s` | Forward / Backward |
| `a` / `d` | Sway-left / Sway-right |
| `i` / `k` | Heave-up / Heave-down |
| `q` | Stop (depth- and yaw-hold) |
| `z` / `x` | Speed +10 / −10 |
| `8` / `5` / `6` / `4` | Increment / decrement target x and y (position-hold extras) |

## 6. How this differs from Programming Assignment #2

The `lab_07_pid_controller` package in this branch is the **instructor
research codebase**. It includes a few things that are NOT permitted
in Programming Assignment #2:

- `msg/MsgTutorial.msg` — a custom message left over from earlier
  prototyping. Assignment 2 forbids custom messages and `message_generation`.
- x and y position-hold PID (keys `8 / 5 / 6 / 4`, command codes
  12–15). Assignment 2 restricts the command set to codes 1–11.
- Three-element `control_target` and `control_state` field layouts
  that match this controller, not the spec-strict layout in the
  Assignment 2 PDF.

**Do not copy this controller as your Assignment 2 submission.** Use
the `assignment2` branch (`assignment2_distribution/name_controller_template/`)
as your starting point for the assignment. This branch is for Lab 07
in-class reference only.

## 7. Honor-code reminder

The whole point of the in-class assignment is to internalise the
cascaded PID structure and the depth / yaw / thruster-mixing logic by
writing it yourself. Copy-pasting this solution into the `lab06`
workspace before you try the TODOs gives you no learning value and
contradicts the purpose of the lab.

If you got stuck mid-way, the better move is usually:

1. Re-read the TODO block carefully (it has slide page references).
2. Re-read the worked example branch right above the TODO (depth PID
   has hints; yaw and thruster mixing have at least the structure
   sketched).
3. Then come back here only as a last-resort comparison.
