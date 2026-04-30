# Programming Assignment #2 — Starter Template

POSTECH CITE700X Mobile Robotics (2026 Spring).
**Due: May 14, 2026 (Thursday) 11:59 PM** (PLMS).

This branch holds a starter template for Programming Assignment #2.
Lab 07 in-class taught you the same concepts with worked examples;
**Assignment 2 is the independent re-implementation**, with fewer hints
and a stricter package spec.

> Read `2026_CITE700X_Assignment2.pdf` (distributed on PLMS) for the
> binding spec. This README only summarises and ships the template
> skeleton.

---

## 1. What's in this branch

```
assignment2_template/
├── README.md                          # this file
└── name_controller_template/
    ├── package.xml                    # <-- rename + edit maintainer
    ├── CMakeLists.txt                 # <-- rename
    ├── Readme.txt                     # <-- fill (commands to run your package)
    └── src/
        ├── auv_controller_node.cpp    # spec-strict skeleton
        └── teleop_keyboard.cpp        # spec-strict skeleton
```

The template is **one ROS package**, not a workspace. Drop it into the
catkin workspace you built for Lab 07 (`~/uuv_ws`) — the UUV simulator
stack and the Cyclops model are already there from the `lab06` branch,
including the IMU plugin swap.

---

## 2. How to use this template

### Step 1 — clone next to your existing Lab 07 workspace

```bash
cd ~
git clone -b assignment2 https://github.com/HERO-Lab-POSTECH/mobile_robotics.git assignment2_template
```

### Step 2 — rename the package to your own name

The submission spec requires `<name>_controller`. Replace `name`
with your own (lowercase, no spaces). For example, if your name is
`gildonghong`:

```bash
cd ~/assignment2_template/assignment2_template
mv name_controller_template gildonghong_controller

# Open package.xml and CMakeLists.txt, replace
#     name_controller_template
# with
#     gildonghong_controller
# Update <maintainer> in package.xml with your name + email.
```

### Step 3 — copy the package into your Lab 07 workspace

```bash
cp -r ~/assignment2_template/assignment2_template/gildonghong_controller \
      ~/uuv_ws/src/
cd ~/uuv_ws
catkin_make
source devel/setup.bash
```

### Step 4 — implement and test

Edit the two source files in `src/<name>_controller/src/`:

- `auv_controller_node.cpp` — implement the PID control loops + the
  thruster mixing for every motion state (per Assignment 2 PDF
  §2.4 and §2.8).
- `teleop_keyboard.cpp` — implement the key handler that publishes
  `/cyclops/command` and `/cyclops/control_target` (per §3.3 / §3.4).

Run (in 4 terminals, after the world + Cyclops are up — see Lab 07
notes):

```bash
rosrun <name>_controller auv_controller_node
rosrun <name>_controller teleop_keyboard
```

### Step 5 — capture step-response plots

The spec requires:

- `yaw_control_result.jpg` — 90° yaw step response (must settle within 2 s,
  `|yaw_error| < 5°`).
- `depth_control_result.jpg` — 1 m depth step response (must settle
  within 2 s, `|depth_error| < 0.1 m`).

Suggestion: use `rqt_plot` or `rosbag record /cyclops/RPYD` and plot
with Python (matplotlib). Save as JPG inside the package folder.

### Step 6 — package and submit

```bash
cd ~/uuv_ws/src
zip -r 20240001_GildongHong_Assignment2.zip <name>_controller \
    -x '*/build/*' '*/devel/*' '*/.git/*' '*/__pycache__/*'
```

The archive must contain **only the package folder**. Do not include
`build/`, `devel/`, `.git/`, IDE configs, or simulator logs.

Submit the `.zip` to PLMS before May 14, 2026 23:59.

---

## 3. Differences from Lab 07 in-class

The Lab 07 in-class skeleton (`lab06` branch) gave you:

- Worked example for the `stop` motion-state branch.
- The full quaternion-to-Euler helper.
- The yaw wrap-around correction code.
- Pre-defined `pos_xy_control` advanced mode (out of scope here).

Assignment 2 strips most of the above. You will write:

- Both PID laws (depth and the cascaded yaw loop) **from scratch**,
  including anti-windup constraints.
- All five motion-state branches in the thruster mixer (`stop`,
  forward, backward, sway-left, sway-right) — no worked example.
- The `teleop_keyboard` key dispatcher (no template helper besides
  the `init_keyboard / _kbhit / _getch` POSIX scaffolding).
- The `Readme.txt` listing every command needed to run your package.

Also note these spec deltas relative to the Lab 07 instructor solution:

| Item | Lab 07 instructor | Assignment 2 spec |
|---|---|---|
| Custom messages | `MsgTutorial.msg` | **None allowed** — the template has no `msg/` folder |
| `control_target` data layout | 5 fields (yaw, depth, vel, x, y) | **3 fields** (yaw, depth, vel) |
| `control_state` data layout | 4 fields | **3 fields** (motion, yaw_ctrl, depth_ctrl) |
| Command codes | 1–15 | **1–11 only** |
| Position-hold PID (X/Y) | included | **out of scope** |
| Target-depth keys | none | `o` / `l` (+0.1 / −0.1 m) |

---

## 4. Spec quick-reference (read the PDF for binding language)

### Subscribed topics (`auv_controller_node`)

| Topic | Type |
|---|---|
| `/cyclops/pressure` | `sensor_msgs/FluidPressure` |
| `/cyclops/imu` | `sensor_msgs/Imu` |
| `/cyclops/command` | `std_msgs/UInt8` |
| `/cyclops/control_target` | `std_msgs/Float32MultiArray` |

### Published topics (`auv_controller_node`)

| Topic | Type |
|---|---|
| `/cyclops/control_state` | `std_msgs/UInt8MultiArray` |
| `/cyclops/thrusters/X/input` (X=0..7) | `uuv_gazebo_ros_plugins_msgs/FloatStamped` |
| `/cyclops/RPYD` | `std_msgs/Float32MultiArray` |

### Loop rates

- Yaw control: **125 Hz** (driven by `/cyclops/imu` callback)
- Depth control: **10 Hz** (driven by `/cyclops/pressure` callback)
- ROS-loop publish rate: 100 Hz

### Command codes (`/cyclops/command`)

| Code | Action | Code | Action |
|---|---|---|---|
| 1 | Forward | 7 | Yaw control OFF |
| 2 | Backward | 8 | Depth control ON |
| 3 | Sway left | 9 | Depth control OFF |
| 4 | Sway right | 10 | Stop all thrusters |
| 5 | Stop | 11 | Yaw initialization (set current yaw to 0) |
| 6 | Yaw control ON | | |

### `/cyclops/control_target` (Float32MultiArray)

| index | meaning | unit |
|---|---|---|
| `data[0]` | target yaw | rad |
| `data[1]` | target depth | m |
| `data[2]` | translational velocity | unitless |

### `/cyclops/control_state` (UInt8MultiArray)

| index | meaning |
|---|---|
| `data[0]` | motion state (1 fwd, 2 bwd, 3 sway-L, 4 sway-R, 0 stop) |
| `data[1]` | yaw control state (0 off, 1 on) |
| `data[2]` | depth control state (0 off, 1 on) |

### `/cyclops/RPYD` (Float32MultiArray)

| index | meaning | unit |
|---|---|---|
| `data[0]` | roll | rad |
| `data[1]` | pitch | rad |
| `data[2]` | yaw | rad |
| `data[3]` | depth | m |

### Performance requirements

| Test | Target | Settling time | Convergence |
|---|---|---|---|
| Yaw step | 90° | ≤ 2 s | `|yaw_error| < 5°` |
| Depth step | 1 m | ≤ 2 s | `|depth_error| < 0.1 m` |

Translational motion (forward/backward/sway) is implemented as an
**additive offset** on the relevant thrusters — not an independent
control loop. With `velocity = 0.1`, forward motion adds ±0.1 to the
two surge-direction thrusters.

---

## 5. Submission checklist

- [ ] Source files in C++ or Python.
- [ ] `package.xml` + `CMakeLists.txt` updated with package name + maintainer.
- [ ] `Readme.txt` listing every command needed to launch your package.
- [ ] `yaw_control_result.jpg` (90° step, settles within 2 s).
- [ ] `depth_control_result.jpg` (1 m step, settles within 2 s).
- [ ] Archive named `<StudentID>_<Name>_Assignment2.zip`.
- [ ] No `build/`, `devel/`, `.git/`, `.vscode/`, `__pycache__/`, ROS bags.
- [ ] Uploaded to PLMS before **May 14, 2026, 23:59**.

---

## 6. Contact

For questions or clarifications: `luckkim123@postech.ac.kr`.
