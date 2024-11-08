source devel/setup.bash
catkin_make
roscore
roslaunch rosbridge_server rosbridge_websocket.launch

rosrun FP_Magang node1
rosrun FP_Magang node2
rosrun FP_Magang node3
