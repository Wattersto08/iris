import rclpy
from rclpy.node import Node

from std_msgs.msg import String



class MinimalSubscriber(Node):
    

    # create file path for today's data here, check to see if one already exists 

    # if location is already found, check to see the last availible image and continue count from there else start from zero  

    image_count = 0

    def __init__(self):
        
        super().__init__('DSLR_sub')
        self.subscription = self.create_subscription(
            String,
            'DSLR_control',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning
        
    
    def listener_callback(self, msg):
        
        if msg.data == 'CAPTURE':
            self.get_logger().info('Capturing Image')
            filename = hex(MinimalSubscriber.image_count) # creates hex string for file name 

            # add capture image control here
    
            self.get_logger().info('Image Captured')

            # save image to disk here 

            self.get_logger().info('Saved as: "%s"' % filename)

            MinimalSubscriber.image_count = MinimalSubscriber.image_count + 1
        else:
            self.get_logger().warn('Enter Valid Input')


def main(args=None):
    
    rclpy.init(args=args)

    DSLR_sub = MinimalSubscriber()

    rclpy.spin(DSLR_sub)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    DSLR_sub.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
