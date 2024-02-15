# Iris

## Todo:

- [ ] add homing routine to the controller hardware 
- [ ] verify home position prior to the subscribers launching
- [ ] add redundancy to the DSLR handler 
- [ ] when the teensy goes to debug mode, restart the teensy to reinstigate the connection to the uros agen
- [ ] train further haar cascades 
- [ ] optimise CV module -> rewrite in c++? 
- [ ] add tracking parameters 




## nice to haves:

- add additional topic for hardware for mode control for example 
    *int topic*
  
   | Mode           | value |
   |----------------|-------|
   | standbye       |   0   |
   | standbye_homed |   1   |
   | home X         |   2   |
   | home Y         |   3   |
   |                |       |

- update camera stack to use arducam 16MP autofocus (via csi)

## Complete:

- [x] Rebuild DSLR handler -  
- [x] integrate webcam to ros2 - [x] 
- [x] integrate DSLR to ros2 - [x] 
- [x] uros testing -> pub sub working adding hardware functionality to come, motors need testing 
- [x] uros build and upload initial init tests - [x]
- [x] build encoder functionality out - [x]
- [x] implement PID control - [x]

# iris_ws 
The ros2 integration element of the project, 
