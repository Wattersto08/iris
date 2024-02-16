# Iris

Iris is a low cost Hardware solution for high quality data collection. Using a webcam for object location and a DSLR for cropping in high resolution images. The projects long term goals are to service vineyards for grape health monitoring. 

<img src="https://github.com/Wattersto08/iris/blob/main/CAD/Images/iRIS.png">

## ROS 

Utilizing ros2 humble this allows for seamless integration into the Ralph project (a UAV platform for this module to sit on) - this will allow me to quickly connect and disconnect from the platform a single 12V supply and newtwork cable. 

Plans to experiment with the google Coral board will allow for more GPU power for realtime ML. 

### Todo:

- [ ] add aruco tracking, for easier tracking testing
- [ ] add homing routine to the controller hardware 
- [ ] create TF tree for the joint positions
- [ ] update hardware interface to take radian inputs and outputs 
- [ ] create viable URDF for visualisation purposes 
- [ ] seperate out launch files for image and hardware testing
- [ ] verify home position prior to the subscribers launching
- [ ] add redundancy to the DSLR handler 
- [ ] when the teensy goes to debug mode, restart the teensy to reinstigate the connection to the uros agen
- [ ] train further haar cascades 
- [ ] optimise CV module -> rewrite in c++? 
- [ ] add tracking parameters 
- [ ] add SQL data storage for photos and adjacent tables for key information: 
        - GPS coordinates 
        - filename
        - time 
        - temps 
    This will be extended to record presense of desease, leaf burning and growth. 

## PCB hardware 

To go along with the software component of the project I have developed a PCB to interface with the hardware, using a PJRC Teensy 4.1 and uROS. This allows for budget compoents to be used whilst still giving a full PID control loop and ROS integration.

## todo for v1.1

- [ ] standardise mounting holes 
- [ ] fan output 
- [ ] remove one 5V4A supply 
- [ ] replace current descrete logic voltage shifting components into a single IC
- [ ] add a power control mosfet to teensy to allow the pi to control power state 
- [ ] add general GPIO for expansion 
- [ ] look into alternatives for connection to host, (ethernet?)

## v1.0

This serves as an initial protype platform:

<img src="https://github.com/Wattersto08/iris/blob/main/CAD/Images/PCB_top.png" width="400" height="400">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://github.com/Wattersto08/iris/blob/main/CAD/Images/PCB_bottom.png" width="400" height="400">



## general project non critical goals:

- [ ] add additional topic for hardware for mode control for example 
    *int topic*
  
   | Mode           | value |
   |----------------|-------|
   | standbye       |   0   |
   | standbye_homed |   1   |
   | home X         |   2   |
   | home Y         |   3   |

- [ ] update camera stack to use arducam 16MP autofocus (via csi)

## Complete:

- [x] Rebuild DSLR handler 
- [x] integrate webcam to ros2 
- [x] integrate DSLR to ros2 
- [x] uros testing -> pub sub working adding hardware functionality to come, motors need testing 
- [x] uros build and upload initial init tests 
- [x] build encoder functionality out 
- [x] implement PID control 

