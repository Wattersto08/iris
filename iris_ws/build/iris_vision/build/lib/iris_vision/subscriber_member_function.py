import rclpy
from rclpy.node import Node

from std_msgs.msg import String

import logging
import locale
import os
import subprocess
import sys
import gphoto2 as gp
from datetime import datetime

date = datetime.today().strftime('%Y-%m-%d')
datapath = '/home/iris/iris/data/'+date+'/'

# ------------- CURRENTLY WILL FAIL IF TODAY's DATA DIRECTORY IS NOT CREATED - RUN TWICE IF SO 


class MinimalSubscriber(Node):

    
    # create file path for today's data here, check to see if one already exists 
    # if location is already found, check to see the last availible image and continue count from there else start from zero  

    def filepathinit():
        global image_count
        if os.path.isdir(datapath):
            print("Dir Enabled") 
            files = os.listdir(datapath)
            image_count = len(files)
            print('Pictures saved from: '+str(image_count))
        else:
            print("Creating Dir")
            os.mkdir(datapath) 
            MinimalSubscriber.filepathinit()

    
    filepathinit()
   

    def __init__(self):
        
        super().__init__('DSLR_sub')
        self.subscription = self.create_subscription(
            String,
            'DSLR_control',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    
   
    
    def listener_callback(self, msg):
        global image_count
        MinimalSubscriber.image_count = image_count
        if msg.data == 'CAPTURE':
            camera = gp.Camera()
            camera.init()   
            self.get_logger().info('Capturing Image')
            filename = hex(image_count)+'.jpg' # creates hex string for file name 
            
            # add capture image control here
            file_path = camera.capture(gp.GP_CAPTURE_IMAGE)
   
            self.get_logger().info('Camera file path: {0}/{1}'.format(file_path.folder, file_path.name))
   
            target = os.path.join(datapath,filename)
    
            camera_file = camera.file_get(
                file_path.folder, file_path.name, gp.GP_FILE_TYPE_NORMAL)
            camera_file.save(target)

            camera.exit()






            self.get_logger().info('Image Captured')

            # save image to disk here 

            self.get_logger().info('Saved as: "%s"' % filename)
            image_count = image_count + 1
      
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
