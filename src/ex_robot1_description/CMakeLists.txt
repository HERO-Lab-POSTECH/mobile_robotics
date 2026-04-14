cmake_minimum_required(VERSION 3.0.2)
project(ex_robot1_description)

# catkin is the ROS1 build system
find_package(catkin REQUIRED)
catkin_package()

# Install urdf/ and launch/ directories so that
# roslaunch and other tools can find them at runtime.
install(DIRECTORY urdf launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
