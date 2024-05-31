import rclpy
from rclpy.node import Node


from std_msgs.msg import Int32
import math as m 

# "iris/currentX" "iris/currentY" "iris/targetX" "iris/targetY"

class MinimalSubscriber(Node):
    errorY = 0
    currentY = 0
    
    def __init__(self):
        super().__init__('minimal_subscriber')
        
       
        self.errorYsubscription = self.create_subscription(
            Int32,
            'iris/errorY',
            self.errorY_callback,
            10)

        self.errorYsubscription  # prevent unused variable warning

        self.currentYsubscription = self.create_subscription(
            Int32,
            'iris/currentY',
            self.currentY_callback,
            10)
        
        self.currentYsubscription  # prevent unused variable warning

        timer_period = 0.01  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

        self.publisherY = self.create_publisher(Int32, 'iris/targetY',10)
        self.publisherX = self.create_publisher(Int32, 'iris/targetX',10)

    def errorY_callback(self, errorYmsg: Int32):
        
        self.errorY = errorYmsg.data    
        #self.get_logger().info('error Y: '+str(self.errorY))

    def currentY_callback(self, currentYmsg: Int32):
        
        self.currentY = currentYmsg.data
        #self.get_logger().info('current Y: '+str(self.currentY))
    
    def timer_callback(self):

        self.get_logger().info(str(self.currentY+self.errorY))
        targposY = self.currentY+(self.errorY*-1)
        msg = Int32()
        msg.data = targposY
        self.publisherY.publish(msg)
        #msg.data = m.floor(JOY.axes[0]*-400.0)
        #self.publisherX.publish(msg)


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