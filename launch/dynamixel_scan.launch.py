"""Launch the one-shot scanner with typed ROS2 parameters."""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    arguments = [
        ("device_name", "/dev/ttyUSB0", str),
        ("baudrate", "4000000", int),
        ("min_id", "0", int),
        ("max_id", "252", int),
        ("lock_timeout_ms", "20", int),
        ("show_missing", "false", bool),
    ]
    return LaunchDescription(
        [DeclareLaunchArgument(name, default_value=default)
         for name, default, _ in arguments]
        + [Node(
            package="dynamixel_sdk_interface",
            executable="dynamixel_scan_node",
            name="dynamixel_scan",
            output="screen",
            parameters=[{
                name: ParameterValue(LaunchConfiguration(name), value_type=kind)
                for name, _, kind in arguments
            }],
        )]
    )
