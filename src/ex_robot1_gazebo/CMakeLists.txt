cmake_minimum_required(VERSION 3.0.2)
project(ex_robot1_gazebo)

find_package(catkin REQUIRED COMPONENTS
  gazebo_plugins
  gazebo_ros
  xacro
  urdf
  ex_robot1_description
)

catkin_package(
  CATKIN_DEPENDS
    gazebo_plugins
    gazebo_ros
    xacro
    urdf
    ex_robot1_description
)

install(DIRECTORY world launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
