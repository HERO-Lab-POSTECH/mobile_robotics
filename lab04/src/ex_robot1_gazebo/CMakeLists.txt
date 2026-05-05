cmake_minimum_required(VERSION 3.0.2)
project(ex_robot1_gazebo)

# This package depends on Gazebo-ROS integration and the URDF description.
find_package(catkin REQUIRED COMPONENTS
  gazebo_plugins       # Gazebo sensor/actuator plugins
  gazebo_ros           # ROS interface for Gazebo (spawn_model, etc.)
  xacro                # URDF macro processor
  urdf                 # URDF parsing library
  ex_robot1_description  # provides the URDF file
)

catkin_package(
  CATKIN_DEPENDS
    gazebo_plugins gazebo_ros xacro urdf ex_robot1_description
)

# Install world/ and launch/ for runtime access.
install(DIRECTORY world launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
