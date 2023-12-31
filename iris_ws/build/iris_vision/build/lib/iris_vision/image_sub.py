#! /usr/bin/python
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image

class MinimalSubscriber(Node):

    def __init__(self):
        super().__init__('minimal_subscriber')
        
        self.subscription = self.create_subscription(
            Image,
            '/usb_cam/image_raw',
            self.listener_callback,
            qos_profile_sensor_data)
        
        self.subscription  # prevent unused variable warning

    def listener_callback(self):
        print("In callback")

    def main(args=None):
        rclpy.init(args=args)
        minimal_subscriber = MinimalSubscriber()
        rclpy.spin(minimal_subscriber)
       # Destroy the node explicitly
       # (optional - otherwise it will be done automatically
       # when the garbage collector destroys the node object)
      
       

if __name__ == '__main__':
    main()
    rclpy.shutdown()