
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
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg_pub;

// subscriber
rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg_sub;

// publisher and subscriber common
rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_timer_t timer;
unsigned int num_handles = 1 + 1;   // 1 subscriber, 1 publisher

#define RCCHECK(fn){rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// pin defintiions 

// general
#define LED1 = 23
#define LED2 = 22
#define Pushbutton1 = 21
#define Pushbutton2 = 20

#define MEN = 3

// X axis 
#define EncXA = 37
#define EncXB = 36
#define LimSwX = 38

#define M1A = 1
#define M1B = 2
#define M1PWM = 0


// Y axis 
#define EncYA = 35
#define EncYB = 34
#define LimSwY = 33

#define M2A = 4
#define M2B = 5
#define M2PWM = 8



// Variable setup 
const unsigned int timer_timeout = 100;
  

// Error handle loop
void error_loop() {
  while(1) {
    digitalWrite(LED2, !digitalRead(LED2));
    delay(500);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg_pub, NULL));
    msg_pub.data++;
  }
}


void subscription_callback(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED1, (msg->data == 0) ? LOW : HIGH);  
}

void setup() {
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, HIGH); 
  digitalWrite(LED2, LOW); 

  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "iris_control_system", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/current"));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "iris/target"));

  // create timer,

  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, num_handles, &allocator));  
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg_sub, &subscription_callback, ON_NEW_DATA));   

  msg_pub.data = 0;
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}