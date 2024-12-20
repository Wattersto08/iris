
#include <Arduino.h>
#include <Wire.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only avaliable for Arduino framework with serial transport.
#endif


// ros init 

//publisher
rcl_publisher_t publisherX, publisherY;
std_msgs__msg__Int32 msg_pubX, msg_pubY;

// subscriber
rcl_subscription_t subscriberX, subscriberY;
std_msgs__msg__Int32 msg_subX, msg_subY;

// publisher and subscriber common
rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_timer_t timer;

unsigned int num_handles = 2 + 2;   // 2 subscriber, 2 publisher

#define RCCHECK(fn){rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// pin defintiions 

// general
#define LED1 23
#define LED2 22
#define Pushbutton1 21
#define Pushbutton2 20

#define MEN 3

// X axis 
#define EncXA 37
#define EncXB 36
#define LimSwX 38

#define M1A 4
#define M1B 5
#define M1PWM 6

// Y axis 
#define EncYA 35
#define EncYB 34
#define LimSwY 33

#define M2A 2
#define M2B 1
#define M2PWM 0

// Variable setup 
const unsigned int timer_timeout = 100;
  
// Error handle loop
void error_loop() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  while(1) {
    digitalWrite(LED1, !digitalRead(LED1));
    digitalWrite(LED2, !digitalRead(LED2));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisherX, &msg_pubX, NULL));
    msg_pubX.data++;
    RCSOFTCHECK(rcl_publish(&publisherY, &msg_pubY, NULL));
    msg_pubY.data++;
  }
}

void subscription_callbackX(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED1, (msg->data == 0) ? LOW : HIGH);  
}

void subscription_callbackY(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED2, (msg->data == 0) ? LOW : HIGH);  
}




void setup() {
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Pushbutton1,INPUT);
  pinMode(Pushbutton2,INPUT);

  pinMode(MEN,OUTPUT);
  pinMode(M1A,OUTPUT);
  pinMode(M1B,OUTPUT);
  pinMode(M1PWM,OUTPUT);
  pinMode(M2A,OUTPUT);
  pinMode(M2B,OUTPUT);
  pinMode(M2PWM,OUTPUT);

  pinMode(EncYA,INPUT);
  pinMode(EncYB,INPUT);
  pinMode(LimSwY,INPUT);
  pinMode(EncXA,INPUT);
  pinMode(EncXB,INPUT);
  pinMode(LimSwX,INPUT);
  
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "iris_control_system", "", &support));

  // Axis topic init 

  // X axis topic init 
  RCCHECK(rclc_publisher_init_default(
    &publisherX,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/currentX"));

  RCCHECK(rclc_subscription_init_default(
    &subscriberX,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/targetX"));

  // Y axis topic init  
  RCCHECK(rclc_publisher_init_default(
    &publisherY,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/currentY"));

  RCCHECK(rclc_subscription_init_default(
    &subscriberY,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/targetY"));

 // create publisher update timer,
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, num_handles, &allocator));  
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriberX, &msg_subX, &subscription_callbackX, ON_NEW_DATA));   
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriberY, &msg_subY, &subscription_callbackY, ON_NEW_DATA));  

  msg_pubX.data = 0;
  msg_pubY.data = 0;
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}