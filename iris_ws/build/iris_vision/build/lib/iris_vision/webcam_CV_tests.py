# Basic ROS 2 program to subscribe to real-time streaming 
# video from your built-in webcam
# Author:
# - Addison Sears-Collins
# - https://automaticaddison.com
  
# Import the necessary libraries
import rclpy # Python library for ROS 2
from rclpy.node import Node # Handles the creation of nodes
from sensor_msgs.msg import Image # Image is the message type
from cv_bridge import CvBridge # Package to convert between ROS and OpenCV Images
import cv2 # OpenCV library
 
class ImageSubscriber(Node):
    """
    Create an ImageSubscriber class, which is a subclass of the Node class.
    """
    def __init__(self):
        """
        Class constructor to set up the node
        """
        # Initiate the Node class's constructor and give it a name
        super().__init__('image_subscriber')
            
        # Create the subscriber. This subscriber will receive an Image
        # from the video_frames topic. The queue size is 10 messages.
        self.subscription = self.create_subscription(
        Image, 
        'usb_cam/image_raw', 
        self.listener_callback, 
        10)

        self.subscription # prevent unused variable warning
        self.MVpub = self.create_publisher(Image, 'iris/CV_image',10)
 
            # Used to convert between ROS and OpenCV images
        self.br = CvBridge()
    
    def listener_callback(self, inputdata: Image):
        """
        Callback function.
        """
        # Display the message on the console
        #self.get_logger().info('Receiving video frame')
        #self.get_logger().info(str(type(inputdata)))
        # Convert ROS Image message to OpenCV image
        current_frame = self.br.imgmsg_to_cv2(inputdata, desired_encoding='passthrough')

        self.get_logger().info(str(current_frame.shape))
        grey = cv2.cvtColor(current_frame, cv2.COLOR_BGR2GRAY)
         # cv2.line(current_frame,(0,0),(511,511),(255,0,0),5)
       
        self.MVpub.publish(self.br.cv2_to_imgmsg(grey))
        self.get_logger().info('published image')
       

        #i#mage_message = Image()
        #image_message = self.br.cv2_to_imgmsg(current_frame, encoding="passthrough")
        #self.MVpub.publish(image_message)
     
def main(args=None):
  
  # Initialize the rclpy library
  rclpy.init(args=args)
  
  # Create the node
  image_subscriber = ImageSubscriber()
  
  # Spin the node so the callback function is called.
  rclpy.spin(image_subscriber)
  
  # Destroy the node explicitly
  # (optional - otherwise it will be done automatically
  # when the garbage collector destroys the node object)
  image_subscriber.destroy_node()
  
  # Shutdown the ROS client library for Python
  rclpy.shutdown()
  
if __name__ == '__main__':
  main()