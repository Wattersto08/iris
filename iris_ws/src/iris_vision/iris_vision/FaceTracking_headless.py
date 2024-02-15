# Basic ROS 2 program to subscribe to real-time streaming 
# video from your built-in webcam
# Author:
# - Addison Sears-Collins
# - https://automaticaddison.com
  
# Import the necessary libraries
import rclpy # Python library for ROS 2
from rclpy.node import Node # Handles the creation of nodes
from sensor_msgs.msg import CompressedImage # Image is the message type
from std_msgs.msg import Int32
from cv_bridge import CvBridge # Package to convert between ROS and OpenCV Images
import cv2 # OpenCV library
import math as m
trained_face_data = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

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
        CompressedImage, 
        'usb_cam/image_raw', 
        self.listener_callback, 
        10)

        self.subscription # prevent unused variable warning

        self.publisherY = self.create_publisher(Int32, 'iris/errorY',10)
        self.publisherX = self.create_publisher(Int32, 'iris/errorX',10)

        self.MVpub = self.create_publisher(CompressedImage, 'iris/CV_image',10)
        # Used to convert between ROS and OpenCV images
        self.br = CvBridge()
    
    def listener_callback(self, inputdata: CompressedImage):
        current_frame = self.br.CompressedImage (inputdata)
        self.get_logger().info(str(type(current_frame.shape)))
        faces = []
        if len(faces) == 1:
          for (x, y, w, h) in faces:

            cv2.rectangle(current_frame, (x, y), (x+w, y+h), (255, 0, 0), 2)
            
            faceX = x+(0.5*w)
            faceY = y+(0.5*h)

            errorX = (current_frame.shape[1]/2) - faceX 
            errorY = (current_frame.shape[0]/2) - faceY 

            msg = Int32()
            msg.data = m.floor(errorY)
            self.publisherY.publish(msg)
            msg.data = m.floor(errorX)
            self.publisherX.publish(msg)
            
            self.get_logger().info(str('X error: '+ str(errorX) + ' Y error: '+ str(errorY)))

        else:
            self.get_logger().info('No Face Detected!')

        
        image_message.data = self.br.cv2_to_compressed_imgmsg(current_frame)        
        self.MVpub.publish(image_message)

       

       

     
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