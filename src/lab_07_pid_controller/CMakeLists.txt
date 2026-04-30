cmake_minimum_required(VERSION 2.8.3)
project(lab_07_pid_controller)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  nav_msgs
  message_generation
  uuv_gazebo_ros_plugins_msgs
)

add_message_files(FILES MsgTutorial.msg)

generate_messages(DEPENDENCIES std_msgs)

catkin_package(
  LIBRARIES cyclops_pid_controller
  CATKIN_DEPENDS std_msgs sensor_msgs nav_msgs roscpp uuv_gazebo_ros_plugins_msgs
)

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(teleop_keyboard src/teleop_keyboard.cpp)
add_dependencies(teleop_keyboard ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(teleop_keyboard ${catkin_LIBRARIES})

add_executable(auv_controller_node src/auv_controller_node.cpp)
add_dependencies(auv_controller_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(auv_controller_node ${catkin_LIBRARIES})
