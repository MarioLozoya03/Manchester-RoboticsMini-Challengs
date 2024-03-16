import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
from launch import LaunchDescription

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('basic_comms_params'),
        'config',
        'params.yaml'
    )

    node = Node(
        package='basic_comms_params',
        name = 'example_node_params',
        executable='node_params',
        parameters=[config]
    )

    ld = LaunchDescription([node])
    return ld