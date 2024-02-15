import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Joy
from std_msgs.msg import Int32
import math as m 

# "iris/currentX" "iris/currentY" "iris/targetX" "iris/targetY"

class MinimalSubscriber(Node):

    Y_min = -400
    Y_max = 400

    def __init__(self):
        super().__init__('minimal_subscriber')
        
       
        self.subscription = self.create_subscription(
            Joy,
            'joy',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning
        self.publisherY = self.create_publisher(Int32, 'iris/targetY',10)
        self.publisherX = self.create_publisher(Int32, 'iris/targetX',10)

    def listener_callback(self, JOY: Joy):
        msg = Int32()
        msg.data = m.floor(JOY.axes[1]*-400.0)
        self.publisherY.publish(msg)
        msg.data = m.floor(JOY.axes[0]*-400.0)
        self.publisherX.publish(msg)


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()