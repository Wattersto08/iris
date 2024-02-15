from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # launch foxglove web socket 
        Node(
            package='foxglove_bridge',
            namespace='foxglove',
            executable='foxglove_bridge'
        ),
        # launch webcam node 
        Node(
            package='usb_cam',
            namespace='usb_cam',
            executable='usb_cam_node_exe',
            name='usb_cam'
        ),
        # launch DSLR handler node 
        Node(
            package='iris_vision',
            executable='DSLR_handler',
            name='DSLR_handler'
        )#,
        
        #Node(
        #    package='micro_ros_agent',
        #    executable='micro_ros_agent',
        #    name='micro_ros_agent',
        #    arguments=["serial", "--dev","/dev/ttyACM0"],
        #    output="screen"
        #),
        
        #Node(
        #    package='iris_vision',
        #    executable='iris_joy_control',
        #    name='iris_joy_control'
        #)


    ])
