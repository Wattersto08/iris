# Iris

Todo:
- Rebuild DSLR handler 
- integrate webcam to ros2 
- integrate DSLR to ros2 
- uros testing -> pub sub working adding hardware functionality to come, motors need testing 
- uros build and upload initial init tests *complete* 
- build encoder functionality out 
- implement PID control 
- add homing routine 
- update camera stack to use arducam 16MP autofocus (via csi)

NICE TO HAVES:

- add additional topic for hardware for mode control for example 
    *int topic*
  
   | Mode           | value |
   |----------------|-------|
   | standbye       |   0   |
   | standbye_homed |   1   |
   | home X         |   2   |
   | home Y         |   3   |
   |                |       |



# iris_ws 
The ros2 integration element of the project, 
