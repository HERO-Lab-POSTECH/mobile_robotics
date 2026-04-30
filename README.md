# Lab 06 + Lab 07 — UUV Simulator, Cyclops, and PID Control

Student workspace for POSTECH CITE700X Mobile Robotics (2026 Spring).
This branch (`lab06`) covers two consecutive labs:

- **Lab 06** — drive the Cyclops AUV in Gazebo with keyboard teleop
  (`lab_06_package`).
- **Lab 07** — close the loop with a yaw + depth PID controller on top
  of Lab 06 (`lab_07_pid_controller`).

Both labs share the same workspace, the same Cyclops model, and the
same UUV simulator stack — clone once, run both.

---

## 1. Prerequisites

- Ubuntu 20.04 + ROS Noetic Desktop Full (the lab Docker image is fine)
- `git`, `python3`, and a working `catkin_make` toolchain

---

## 2. Clone and build

```bash
# 1. Clone the lab06 branch into ~/uuv_ws
cd ~
git clone -b lab06 https://github.com/HERO-Lab-POSTECH/mobile_robotics.git uuv_ws
cd ~/uuv_ws

# 2. Install apt dependencies (run once)
sudo ./install_deps.sh

# 3. Build the workspace
catkin_make
source devel/setup.bash
```

> The first build succeeds because `lab_06_package` and
> `lab_07_pid_controller` ship as **stubs** — they compile but the AUV
> will not actually move/control until you fill in the TODOs in §4
> (Lab 06) and §12 (Lab 07).

> Add `source ~/uuv_ws/devel/setup.bash` to your `~/.bashrc` if you'd
> like every new terminal to find the workspace automatically.

---

# Lab 06 — UUV Simulator and Cyclops

## 3. Run the simulator

Three terminals (all inside the lab container).

```bash
# Terminal 1 — World (Gazebo + sea floor)
roslaunch uuv_gazebo_worlds heroslam_map.launch
```

Wait until the herkules seabed is fully loaded.

```bash
# Terminal 2 — Spawn Cyclops
roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch
```

You should see the Cyclops AUV inside Gazebo. Verify the topics:

```bash
rostopic list | grep cyclops/thrusters
# /cyclops/thrusters/0/input ... /cyclops/thrusters/7/input must appear.
```

---

## 4. In-class assignment — fill the TODOs

Edit `src/lab_06_package/src/`:

| File | TODO | What to do |
|------|------|------------|
| `command_node.cpp` | TODO 1 | Initialize `ros_pub` with `nh.advertise<std_msgs::Char>("/key_command", 100)` |
| `command_node.cpp` | TODO 2 | Add `ros_pub.publish(msg);` |
| `control_node.cpp` | TODO 1 | Map keys to `control_state` / `control_speed` in `msgCallback` (`q` is given) |
| `control_node.cpp` | TODO 2 | Initialize the 8 thruster publishers (`/cyclops/thrusters/<n>/input`) |
| `control_node.cpp` | TODO 3 | Publish thruster commands for `control_state == 2,3,4,5,6,0` (`forward` is given) |

Refer to slide pages 17–19 for the exact key map and thruster values.

Build and run:

```bash
cd ~/uuv_ws
catkin_make
source devel/setup.bash
```

```bash
# Terminal 3 — control_node
rosrun lab_06_package control_node

# Terminal 4 — command_node (this terminal must keep keyboard focus)
rosrun lab_06_package command_node
```

### Lab 06 controls

| Key | Action |
|-----|--------|
| `w` | Forward |
| `s` | Backward |
| `a` | Sway-left |
| `d` | Sway-right |
| `i` | Heave-up |
| `k` | Heave-down |
| `q` | Stop |
| `z` | Speed +10 |
| `x` | Speed −10 |

Pressing `w` should make Cyclops move forward in Gazebo. That confirms
the Lab 06 in-class assignment is complete.

---

## 5. Known harmless warnings (Lab 06)

The first time Gazebo starts you'll see these — **safe to ignore**:

```
[Wrn] [SystemPaths.cc:460] File or path does not exist [""] [model://bricks1]
... bricks2..4, unit_box ...
```

These are decorative scene props that aren't shipped with the lab
distribution. The simulation itself is unaffected.

---

# Lab 07 — Underwater Robot PID Control

Lab 07 adds a closed-loop yaw + depth PID controller on top of the same
Cyclops simulation. The IMU plugin in `cyclops_sensors.xacro` is
**already pre-configured** to publish `/cyclops/imu` at 125 Hz, which is
what the yaw control loop runs on.

## 6. Sensors used

| Topic | Type | Used for |
|-------|------|----------|
| `/cyclops/pressure` | `sensor_msgs/FluidPressure` | depth (10 Hz) |
| `/cyclops/imu` | `sensor_msgs/Imu` | yaw + yaw-rate (125 Hz) |
| `/cyclops/pose_gt` | `nav_msgs/Odometry` | x/y position (advanced, optional) |

> If `/cyclops/imu` is missing, you cloned the wrong branch or the IMU
> swap was reverted. Re-run `git pull` and rebuild.

## 7. Run sequence (Lab 07)

Reuse Terminals 1 and 2 from Lab 06 (world + Cyclops spawn) — do **not**
launch the Lab 06 nodes. Then in two new terminals:

```bash
# Terminal 3 — PID controller
rosrun lab_07_pid_controller auv_controller_node

# Terminal 4 — teleop (keyboard focus must stay here)
rosrun lab_07_pid_controller teleop_keyboard
```

The teleop window shows the live AUV state (control mode, RPY, depth,
target depth, vehicle velocity) — it `system("clear")`s every loop.

---

## 8. In-class assignment — fill the TODOs (harder than Lab 06)

All TODOs live in
`src/lab_07_pid_controller/src/auv_controller_node.cpp`.

| TODO | Where | What to do |
|------|-------|------------|
| TODO 1 | `#define` block at top | Tune the PID gains (currently stubbed at 1.0). Recommended order: P → D → I. |
| TODO 2a | inside `msgCallbackDepth` | Implement the **depth PID law** (P + I + D, with anti-windup `constrain(I_depth, -500, 500)`). |
| TODO 2b | inside `msgCallbackIMU` | Implement the **cascaded yaw PID law** (outer P on angle → inner PID on yaw rate). |
| TODO 3 | `move_command == 1,2,3,4` branches in `msgCallbackIMU` | Implement thruster mixing for forward / backward / sway-left / sway-right. The `stop` branch is given as a worked example. |

Once filled, rebuild:

```bash
cd ~/uuv_ws && catkin_make && source devel/setup.bash
```

---

## 9. Lab 07 keys

`teleop_keyboard` terminal (must keep focus).

| Key | Action |
|-----|--------|
| `w` / `s` | Forward / Backward |
| `a` / `d` | Sway-left / Sway-right |
| `q` | Stop |
| `y` / `h` | Yaw control ON / OFF |
| `p` / `;` | Depth control ON / OFF |
| `g` | Stop all thrusters (emergency) |
| `n` | Yaw initialization (set current yaw to 0) |
| `i` / `k` | Target yaw +0.1 / −0.1 rad |
| `o` / `l` | Target depth +0.1 / −0.1 m |
| `z` / `x` | Velocity +10 / −10 |

Default `target_depth` = 55.0 m, default `target_yaw` = 0 rad.

---

## 10. What success looks like (Lab 07)

After all TODOs are filled in:

1. Running `auv_controller_node` alone should keep Cyclops still
   (PID holds depth and yaw). If it drifts → revisit TODO 1 / TODO 2.
2. Press `y` (yaw on), then `i` a few times to bump the target yaw to
   ~0.5 rad — Cyclops should rotate to that heading and settle.
3. Press `p` (depth on) — Cyclops should slowly descend toward
   `target_depth = 55 m`.
4. Press `w` — Cyclops moves forward (worked-example branch).
5. Press `s/a/d` — these only work after you finish TODO 3.

---

## 11. Lab 07 troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| `auv_controller_node` runs but Cyclops drifts | TODO 1 stub (1.0) gains, or TODO 2 PID law not yet written | Tune gains and complete the PID computations |
| Yaw oscillates without settling | `YAW_GAIN_D` too small or zero | Increase D until oscillation damps |
| Cyclops floats up uncontrollably | Wrong sign in depth PID or thruster output | Recheck TODO 2a sign convention |
| `s/a/d` keys do nothing | TODO 3 still empty | Implement those branches |
| `/cyclops/imu` is missing | IMU xacro wasn't swapped (wrong branch?) | Re-clone `lab06` branch and rebuild |

---

## 12. Pulling instructor updates

```bash
cd ~/uuv_ws
git stash             # save your in-progress edits
git pull              # fetch instructor updates
git stash pop         # restore your edits
catkin_make && source devel/setup.bash
```

Resolve any conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`) by hand,
then rebuild.

---

## 13. Quick reset

If your build is in a bad state:

```bash
cd ~/uuv_ws
rm -rf build devel
catkin_make
source devel/setup.bash
```

---

## 14. File structure

```
uuv_ws/
├── install_deps.sh
├── README.md                                    # this file
└── src/
    ├── lab_06_package/                          # Lab 06 (TODO)
    │   └── src/
    │       ├── command_node.cpp                 # TODO 1, 2
    │       └── control_node.cpp                 # TODO 1, 2, 3
    ├── lab_07_pid_controller/                   # Lab 07 (TODO)
    │   └── src/
    │       ├── auv_controller_node.cpp          # TODO 1, 2a, 2b, 3
    │       └── teleop_keyboard.cpp              # provided, do not edit
    ├── uuv_cyclops_description/                 # Cyclops URDF (IMU pre-configured for Lab 07)
    ├── uuv_gazebo_worlds/                       # heroslam world (do not edit)
    └── (10 other uuv_simulator packages)        # do not edit
```

---

## 15. Looking ahead — Programming Assignment 2

Programming Assignment #2 (PLMS) is a stricter, **independent**
re-implementation of the Lab 07 controller without worked examples.
A starter template is published on the **`assignment2`** branch of this
repository:

```bash
git clone -b assignment2 https://github.com/HERO-Lab-POSTECH/mobile_robotics.git assignment2_template
```

See `assignment2/README.md` on that branch for the full spec and
submission rules. You will likely want to finish the Lab 07 in-class
work first — the assignment expects you to know how the cascaded yaw
PID and depth PID laws work, but it does **not** give them to you.
