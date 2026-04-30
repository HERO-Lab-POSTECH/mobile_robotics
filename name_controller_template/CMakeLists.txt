cmake_minimum_required(VERSION 3.0.2)

# ============================================================================
# Programming Assignment #2 starter template.
# Rename this project to match the package name (e.g., gildonghong_controller).
# ============================================================================
project(name_controller_template)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  uuv_gazebo_ros_plugins_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs sensor_msgs uuv_gazebo_ros_plugins_msgs
)

include_directories(${catkin_INCLUDE_DIRS})

# Per Assignment 2 spec: node names are fixed.
add_executable(auv_controller_node src/auv_controller_node.cpp)
target_link_libraries(auv_controller_node ${catkin_LIBRARIES})

add_executable(teleop_keyboard src/teleop_keyboard.cpp)
target_link_libraries(teleop_keyboard ${catkin_LIBRARIES})
