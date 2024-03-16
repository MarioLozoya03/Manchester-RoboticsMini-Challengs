import rclpy
from rclpy.node import Node
from msg_clase.msg import Sphere
import numpy as np

class CustomMsgPublisher(Node):
    def __init__(self):
        super().__init__('custom_msg_node')
        self.pub = self.create_publisher(Sphere, 'sphere_position', 10)
        self.timer_period = 0.1
        self.timer = self.create_timer(self.timer_period, self.timer_callback)
        self.x = 0
        self.y = 0
        self.z = 0
        self.r = 1.5
        self.time = 0
        self.get_logger().info('Custom Sphere msg initialized')

    def timer_callback(self):
        msg = Sphere()
        self.r = 1.5 + 0.5*np.sin(self.time)
        self.time += self.timer_period
        msg.radius = self.r
        self.pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    c_m_p = CustomMsgPublisher()
    rclpy.spin(c_m_p)
    c_m_p.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()