import rclpy
import numpy as np
from rclpy.node import Node
from std_msgs.msg import Float32

class SignalGenerator(Node):
    
    def __init__(self):
        super().__init__('signal_generator')
        self.publisher_signal = self.create_publisher(Float32, '/signal', 10)
        self.publisher_time = self.create_publisher(Float32, '/time', 10)
        timer_period = 0.1
        self.timer = self.create_timer(timer_period,self.generate_signal)
        self.get_logger().info('Signal_generator node succesfully initialized')
        self.t = 0.0
        self.msg_signal = Float32()
        self.msg_time = Float32()

    def generate_signal(self):
        self.msg_signal.data = np.sin(self.t)
        self.publisher_signal.publish(self.msg_signal)

        self.msg_time.data = self.t
        self.publisher_time.publish(self.msg_time)

        self.get_logger().info("Time: %f, Signal: %f" % (self.msg_time.data, self.msg_signal.data))

        self.t += 0.1

    
def main(args=None):
    rclpy.init(args=args)
    signal_generator = SignalGenerator()
    rclpy.spin(signal_generator)
    signal_generator.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
