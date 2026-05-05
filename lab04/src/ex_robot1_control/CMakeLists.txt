cmake_minimum_required(VERSION 3.0.2)
project(ex_robot1_control)

# This is a config-only package (no C++/Python code to build).
# It just provides YAML config and launch files.
find_package(catkin REQUIRED)
catkin_package()

# Install config/ and launch/ so roslaunch can find them at runtime.
install(DIRECTORY config launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
