
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

#define TWO_PI 6.283185307179586476925286766559

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
const byte LED1 = 23;
const byte LED2 = 23;

const byte Xlimsw_pin = 38;
const byte XencA_pin = 37;
const byte XencB_pin = 36;

const byte Ylimsw_pin = 33;
const byte YencA_pin = 35;
const byte YencB_pin = 34;

const byte YmotA_pin = 1;
const byte YmotB_pin = 2;
const byte YmotPWM_pin = 0;

const byte XmotA_pin = 4;
const byte XmotB_pin = 5;
const byte XmotPWM_pin = 6;

const byte mot_enable_pin = 3;

// temporary values for Set_motor_power()

int dirA;
int dirB;
int pwm;

// Limit Switch Variable Setup 

unsigned long limswX_interrupt_time = 0;
unsigned long limswX_interrupt_time_prev = 0;

unsigned long limswY_interrupt_time = 0;
unsigned long limswY_interrupt_time_prev = 0;

unsigned long limsw_debounce_time = 300;

// Encoder Variable Setup

long X_pos = 0;
int X_Counts_per_rev = 2048;

long Y_pos = 0;
int Y_Counts_per_rev = 2048;

// PID Variable Setup

bool PID_debug = false;

float X_kp = 1.5;
float X_kd = 1.5;
float X_ki = 0.015;

long prevT = 0;

float X_eprev = 0;
float X_eintegral = 0;

float Y_kp = 1.2;
float Y_kd = 0.0;
float Y_ki = 0.0;

float Y_eprev = 0;
float Y_eintegral = 0;

int X_target = 0;
int Y_target = 0;

// Function Init 

void X_limsw_interrupt();
void X_encoder_interrupt();

void Y_limsw_interrupt();
void Y_encoder_interrupt();

void Set_motor_power(bool Mselect, int pwr);
void Disable_motor();

float enc_count_to_rad(float counts_per_rev, float curr_count){
  return ((curr_count/counts_per_rev)*TWO_PI);
}

float rad_to_enc_count(float counts_per_rev, float input_rad){
  return int((input_rad/TWO_PI)*counts_per_rev);
}

// PID Setup 
IntervalTimer PID_Timer;

void Update_PID(){

  long currT = micros();
  float deltaT = ((float) (currT - prevT)) / ( 1.0e6 );
  prevT = currT;

  // error
  noInterrupts();
  int Xe = X_pos - X_target;
  int Ye = Y_pos - Y_target;
  interrupts();
  
  // derivative
  float Xdedt = (Xe - X_eprev) / (deltaT);
  float Ydedt = (Ye - Y_eprev) / (deltaT);

  // integral
  X_eintegral = X_eintegral + Xe * deltaT;
  Y_eintegral = Y_eintegral + Ye * deltaT;

  // control signal
  float Xu = (X_kp * Xe) + (X_kd * Xdedt) + (X_ki * X_eintegral);
  float Yu = (Y_kp * Ye) + (Y_kd * Ydedt) + (Y_ki * Y_eintegral);

 
  Set_motor_power(1, floor(Xu));
  Set_motor_power(0, floor(Yu));

  // store previous error
  X_eprev = Xe;
  Y_eprev = Ye;

}

// This Variable will need to be tuned to run as fast as possible - Once PID calculation timing is worked out this can be set with some headroom. 
int PID_time_interval = 100000; // Sets timer interval in ns for PID calculations 


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
  X_target = msg->data;
}

void subscription_callbackY(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  Y_target = msg->data;
}




void setup() {
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(Xlimsw_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Xlimsw_pin), X_limsw_interrupt, RISING);

  pinMode(XencA_pin, INPUT_PULLUP);
  pinMode(XencB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(XencA_pin), X_encoder_interrupt, RISING);

  pinMode(Ylimsw_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Ylimsw_pin), Y_limsw_interrupt, RISING);

  pinMode(YencA_pin, INPUT_PULLUP);
  pinMode(YencB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(YencA_pin), Y_encoder_interrupt, RISING);
  
  // Initialise PID Timer
  PID_Timer.begin(Update_PID, PID_time_interval); 

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

void X_limsw_interrupt() {
  limswX_interrupt_time = millis();
  if((limswX_interrupt_time - limswX_interrupt_time_prev)>limsw_debounce_time){
    delay(100);
    if(digitalRead(Xlimsw_pin)== HIGH){
      X_pos = 0;
    }
  }
  limswX_interrupt_time_prev = limswX_interrupt_time;
}

void Y_limsw_interrupt() {
  limswY_interrupt_time = millis();
  if((limswY_interrupt_time - limswY_interrupt_time_prev)>limsw_debounce_time){
    delay(100);
    if(digitalRead(Ylimsw_pin)== HIGH){
      Y_pos = 0;
    }
  }
  limswY_interrupt_time_prev = limswY_interrupt_time;
}

void X_encoder_interrupt(){
  if(digitalRead(XencB_pin) == HIGH){
    X_pos++;
  } else {
    X_pos--;
  }
}

void Y_encoder_interrupt(){
  if(digitalRead(YencB_pin) == HIGH){
    Y_pos++;
  } else {
    Y_pos--;
  }
}

void Set_motor_power(bool Mselect, int pwr){
/* 
  motor pwr between -256 -> 256 
  Mselect == TRUE  -> X axis
  Mselect == FALSE  -> Y axis
*/

// enable motor
  digitalWrite(mot_enable_pin,HIGH);

// Select Motor
  if(Mselect){
    dirA = XmotA_pin;
    dirB = XmotB_pin;
    pwm = XmotPWM_pin;
  }else{
    dirA = YmotA_pin;
    dirB = YmotB_pin;
    pwm = YmotPWM_pin;
  }

// Set direction Pins 
  if(pwr >= 0){
    digitalWrite(dirA,HIGH);
    digitalWrite(dirB,LOW);
  }else{
    digitalWrite(dirA,LOW);
    digitalWrite(dirB,HIGH);
  }

// Write Power 
  analogWrite(pwm,abs(pwr));
}

void Disable_motor(){
  // simple Call to disable Motors 
  digitalWrite(mot_enable_pin,LOW);
}