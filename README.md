# Lab 06 — Reference Solution (instructor key)

POSTECH CITE700X Mobile Robotics (2026 Spring).
Branch: **`lab06_solution`** of `HERO-Lab-POSTECH/mobile_robotics`.

> **Stop. Read this first.**
>
> This branch contains the **complete reference solution** for the
> Lab 06 in-class assignment (Cyclops keyboard teleop). It is **not**
> the workspace you should clone before the lab.
>
> Use the **`lab06`** branch to work the in-class assignment yourself
> first. Only consult this branch *after* you have attempted the
> assignment, to compare your implementation or to recover from a
> blocked spot.

---

## 1. What's in this branch

```
lab06_solution/
├── install_deps.sh
├── README.md                                     # this file
└── src/
    ├── lab_06_package/                           # FULL implementation (all TODOs filled)
    │   └── src/
    │       ├── command_node.cpp                  # publisher + publish call complete
    │       └── control_node.cpp                  # all 9 keys + 8 thruster pubs +
    │                                             # all 7 motion-state branches
    ├── uuv_cyclops_description/                  # Cyclops URDF (IMU plugin pre-applied
    │                                             # for forward-compat with Lab 07)
    ├── uuv_gazebo_worlds/                        # heroslam world
    └── (10 other UUV simulator packages)         # unchanged from lab06 branch
```

This branch deliberately **does not** include `lab_07_pid_controller`
or the Lab 07 solution. Lab 07 has a separate solution branch (TBD)
and should not be referenced until you finish the Lab 07 in-class
assignment yourself.

## 2. How to use this branch (after attempting the lab)

### Option A — quick comparison (do not overwrite your work)

```bash
git clone -b lab06_solution https://github.com/HERO-Lab-POSTECH/mobile_robotics.git uuv_ws_solution
diff -ru ~/uuv_ws/src/lab_06_package/src ~/uuv_ws_solution/src/lab_06_package/src
```

### Option B — fresh-build the solution (separate workspace)

```bash
cd ~
git clone -b lab06_solution https://github.com/HERO-Lab-POSTECH/mobile_robotics.git uuv_ws_solution
cd ~/uuv_ws_solution
sudo ./install_deps.sh        # safe to skip if already done for Lab 06
catkin_make
source devel/setup.bash
```

Then run the same four-terminal sequence as in the Lab 06 student
README:

```bash
# Terminal 1
roslaunch uuv_gazebo_worlds heroslam_map.launch
# Terminal 2
roslaunch uuv_cyclops_description upload_cyclops_heroslam.launch
# Terminal 3
rosrun lab_06_package control_node
# Terminal 4 (keyboard focus)
rosrun lab_06_package command_node
```

Cyclops should respond to `w / s / a / d / i / k / q` (motion) and
`z / x` (speed) keys.

## 3. Differences vs. the `lab06` student branch

| File | `lab06` student branch | `lab06_solution` (this branch) |
|---|---|---|
| `src/lab_06_package/src/command_node.cpp` | TODO 1, 2 left as stubs | publisher initialised + publish call active |
| `src/lab_06_package/src/control_node.cpp` | only `q` and `w` keys mapped; only `pub_thruster0/1` initialised; only `forward` and `stop` branches implemented | all 9 keys mapped; all 8 thruster publishers initialised; all 7 motion-state branches implemented |
| Everything else | identical | identical |

The Cyclops URDF, the world file, and every UUV simulator package are
unchanged.

## 4. Key bindings (same as student branch)

| Key | Action |
|---|---|
| `w` / `s` | Forward / Backward |
| `a` / `d` | Sway-left / Sway-right |
| `i` / `k` | Heave-up / Heave-down |
| `q` | Stop |
| `z` / `x` | Speed +10 / −10 |

## 5. Honor-code reminder

The whole point of the in-class assignment is to internalise the ROS
publisher / subscriber pattern by writing it yourself. Copy-pasting
this solution into the `lab06` workspace before you try the TODOs
gives you no learning value and contradicts the purpose of the lab.

If you got stuck mid-way, the better move is usually:

1. Re-read the TODO block carefully (it has slide page references).
2. Re-read the worked example branch right above the TODO.
3. Then come back here only as a last-resort comparison.
