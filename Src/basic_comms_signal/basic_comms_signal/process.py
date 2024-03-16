import rclpy
import math
from rclpy.node import Node
from std_msgs.msg import Float32

class Process(Node):
    def __init__(self):
        super().__init__('process')
        self.subscription_signal = self.create_subscription(Float32, '/signal', self.signal_callback, 10)
        self.subscription_time = self.create_subscription(Float32, '/time', self.time_callback, 10)
        self.publisher_proc_signal = self.create_publisher(Float32, '/proc_signal', 10)
        timer_period = 0.1
        self.timer = self.create_timer(timer_period, self.procesed_signal)
        self.msg_proc_signal = Float32()
        self.t = 0.0
        self.get_logger().info('Process node initialized!!!')

    def signal_callback(self, msg):
        # Almacenar el valor de la señal senoidal recibida
        self.msg_proc_signal.data = msg.data

    def time_callback(self, msg):
        self.get_logger().info("Time: %f" % msg.data)
        self.t = msg.data

    def procesed_signal(self):
        # Agregar el offset y reducir la amplitud al valor de la señal senoidal almacenado
        #processed_value = (((self.msg_proc_signal.data) * math.cos(math.pi/2) + math.sqrt(1-(self.msg_proc_signal.data)*(self.msg_proc_signal.data)) * math.sin(math.pi/2)) * 0.5) + 0.5
        processed_value = (((self.msg_proc_signal.data) * math.cos(math.pi/2) + math.cos(self.t) * math.sin(math.pi/2)) * 0.5) + 0.5

        # Crear un nuevo mensaje Float32 y asignarle el valor procesado
        self.msg_proc_signal.data = processed_value
        # Publicar el mensaje procesado en el tópico '/proc_signal'
        self.publisher_proc_signal.publish(self.msg_proc_signal)
        self.get_logger().info("Processed Signal: %f" % processed_value)

def main(args=None):
    rclpy.init(args=args)
    process = Process()
    rclpy.spin(process)
    process.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()