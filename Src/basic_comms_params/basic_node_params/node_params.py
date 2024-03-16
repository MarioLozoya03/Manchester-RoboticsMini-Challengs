import rclpy
from rclpy.node import Node

class TestYAMLParams(Node):
    
    def __init__(self):
        super().__init__('example_node_params')
        self.declare_parameters(
            namespace='',
            parameters=[
                ('bool_value', rclpy.Parameter.Type.BOOL), ('int_number', rclpy.Parameter.Type.INTEGER),
                ('float_number', rclpy.Parameter.Type.DOUBLE), ('str_text', rclpy.Parameter.Type.STRING),
                ('bool_array', rclpy.Parameter.Type.BOOL_ARRAY), ('int_array', rclpy.Parameter.Type.INTEGER_ARRAY),
                ('float_array', rclpy.Parameter.Type.DOUBLE_ARRAY), ('str_array', rclpy.Parameter.Type.STRING_ARRAY),
                ('bytes_array', [0x00]), ('nested_param.another_int', rclpy.Parameter.Type.INTEGER )
            ])
        
def main(args=None):
    rclpy.init(args=args)
    node = TestYAMLParams()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()