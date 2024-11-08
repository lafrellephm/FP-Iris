cmake_minimum_required(VERSION 3.0.2)
project(FP_Magang)

## Compile as C++11
add_compile_options(-std=c++11)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  message_generation
  cv_bridge
  image_transport
)

## Find OpenCV
find_package(OpenCV REQUIRED)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  PC2BS.msg
  BS2PC.msg
)

## Generate added messages and services
generate_messages(
  DEPENDENCIES
  std_msgs
)

###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES FP_Magang
  CATKIN_DEPENDS roscpp rospy message_runtime std_msgs
)

###########
## Build ##
###########

## Specify additional locations of header files
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

## Declare C++ executables
add_executable(node1 src/node1.cpp)
add_executable(node2 src/node2.cpp)
add_executable(node3 src/node3.cpp)

## Link the executable with catkin and OpenCV libraries
target_link_libraries(node1
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

target_link_libraries(node2
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

target_link_libraries(node3
  ${catkin_LIBRARIES}
)

## Add dependencies to the messages for each node
add_dependencies(node1 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(node2 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(node3 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
