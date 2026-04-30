#!/usr/bin/env bash
set -euo pipefail

echo "[install_deps] Installing Lab 06 apt dependencies..."

apt-get update
apt-get install -y \
  ros-noetic-gazebo-ros-pkgs \
  ros-noetic-gazebo-ros-control \
  ros-noetic-robot-state-publisher \
  ros-noetic-joint-state-publisher \
  ros-noetic-xacro \
  ros-noetic-effort-controllers \
  protobuf-compiler \
  libprotobuf-dev

echo "[install_deps] Done."
