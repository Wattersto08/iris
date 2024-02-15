
/* reference file from foxy implementation:

- version had working timer interrupt for PID calculation 
- working uros implementation 


*/
#include <micro_ros_platformio.h>
#include <Arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <Encoder.h>

const byte Enable = 25;

// Pan GPIO
const byte PWMB = 29;
const byte BI2 = 31;
const byte BI1 = 30;


const byte BENCA = 14;
const byte BENCB = 15;

// Tilt GPIO
const byte PWMA = 28;
const byte AI2 = 27;
const byte AI1 = 26;

const byte AENCA = 36;
const byte AENCB = 37;

//limswitch GPIO
const byte limSwA = 40;
const byte limSwB = 18;

int tilt_homeoffset = 5000;
int pan_homeoffset = 10000;

float PAN_kp = 1.5;
float PAN_kd = 1.5;
float PAN_ki = 0.015;

long PAN_prevT = 0;
float PAN_eprev = 0;
float PAN_eintegral = 0;

float TILT_kp = 1.2;
float TILT_kd = 0;
float TILT_ki = 0;

/*
float TILT_kp = 5;
float TILT_kd = 0.1;
float TILT_ki = 0.01;
*/


long TILT_prevT = 0;
float TILT_eprev = 0;
float TILT_eintegral = 0;

 int PAN_target = 0;//-2500*sin(PAN_prevT/1e6); // PAN Limits +/- 10000
 int TILT_target = 0;//(2500*sin(TILT_prevT/1e6)); // tilt // low limit:800 High Limit: 1500


Encoder TILT_ENC(BENCA, BENCB);
Encoder PAN_ENC(AENCA, AENCB);

long pan_pos  = 0;
long pan_newpos;
long tilt_pos  = 0;
long tilt_newpos;


int homespeed = 200;


rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

rcl_subscription_t subscriberP, subscriberT;
rcl_publisher_t publisherP, publisherT;
std_msgs__msg__Int32 msg_tilt, msg_pan, msg_cur_tilt, msg_cur_pan;
rclc_executor_t executorT, executorP, executorCT, executorCP;


#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

IntervalTimer PID_UpdateTimer;


void error_loop(){
  for(int i = 0; i<20; i++){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
  SCB_AIRCR = 0x05FA0004;
}

void Tilt_callback(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg_tilt = (const std_msgs__msg__Int32 *)msgin;
  //digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);  
  TILT_target = (msg_tilt->data)*-1;
}

void Pan_callback(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg_pan = (const std_msgs__msg__Int32 *)msgin;
  //digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);  
  PAN_target = msg_pan->data;
}

void Pos_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    msg_cur_pan.data = pan_newpos;
    msg_cur_tilt.data = tilt_pos;
    RCSOFTCHECK(rcl_publish(&publisherP, &msg_cur_pan, NULL));
    RCSOFTCHECK(rcl_publish(&publisherT, &msg_cur_tilt, NULL));
  }
}


void Set_Motor_Power(int M_en, int Min1, int Min2, int power) {

  power = constrain(power, -255, 255);

  if (power > 0) {
    digitalWrite(Min1, LOW);
    digitalWrite(Min2, HIGH);
    analogWrite(M_en, power);
  }
  else {
    digitalWrite(Min1, HIGH);
    digitalWrite(Min2, LOW);
    analogWrite(M_en, abs(power));
  }

}


void home_tilt() {
  Serial.println("Homing Tilt...");
  bool homedir = digitalRead(limSwB);
  int homepwr = homespeed;
  if (homedir) {
    homepwr = -homespeed;
  }
  while (1) {
    Set_Motor_Power(PWMB, BI2, BI1, homepwr);

    if (digitalRead(limSwB) != homedir) {
      // Reset Tilt Encoder Value here
      TILT_ENC.write(0);//tilt_homeoffset);
      Serial.println("-- Tilt Homed!");
      break;
    }
    delay(10);
  }
  Set_Motor_Power(PWMB, BI2, BI1, 0);
  delay(100);
}

void home_pan() {
  Serial.println("Homing Pan...");
  bool homedir = digitalRead(limSwA);
  int homepwr = homespeed;
  if (homedir) {
    homepwr = -homespeed;
  }
  while (1) {
    Set_Motor_Power(PWMA, AI2, AI1, homepwr);

    if (digitalRead(limSwA) != homedir) {
      // Reset Tilt Encoder Value here
      PAN_ENC.write(pan_homeoffset);
      Serial.println("-- Pan Homed!");
      break;
    }
    delay(10);
  }
  Set_Motor_Power(PWMA, AI2, AI1, 0);
  delay(100);
}

void Update_PID(){

  long PAN_currT = micros();
  float PAN_deltaT = ((float) (PAN_currT - PAN_prevT)) / ( 1.0e6 );
  PAN_prevT = PAN_currT;

  long TILT_currT = micros();
  float TILT_deltaT = ((float) (TILT_currT - TILT_prevT)) / ( 1.0e6 );
  TILT_prevT = TILT_currT;

  
  pan_newpos = PAN_ENC.read();

  if (pan_newpos != pan_pos) {
    pan_pos = pan_newpos;
  }

  
  tilt_newpos = TILT_ENC.read();

  if (tilt_newpos != tilt_pos) {
    tilt_pos = tilt_newpos;
  }

  // error
  int Pe = pan_pos - PAN_target;
  int Te = tilt_pos - TILT_target;

  // derivative
  float Pdedt = (Pe - PAN_eprev) / (PAN_deltaT);
  float Tdedt = (Te - TILT_eprev) / (TILT_deltaT);

  // integral
  PAN_eintegral = PAN_eintegral + Pe * PAN_deltaT;
  TILT_eintegral = TILT_eintegral + Te * TILT_deltaT;

  // control signal
  float Pu = PAN_kp * Pe + PAN_kd * Pdedt + PAN_ki * PAN_eintegral;
  float Tu = TILT_kp * Te + TILT_kd * Tdedt + TILT_ki * TILT_eintegral;


  // signal the motor
  Set_Motor_Power(PWMB, BI1, BI2, floor(Tu));
  Set_Motor_Power(PWMA,AI1, AI2, floor(Pu));


  // store previous error
  PAN_eprev = Pe;
  TILT_eprev = Te;

}

void setup() {
  
  pinMode(PWMB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(limSwB, INPUT_PULLUP);


  digitalWrite(Enable, HIGH);
  delay(100);
  home_tilt();
  //home_pan();
 
  set_microros_serial_transports(Serial);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriberT,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),"/iRIS/Tilt_targ"));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriberP,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),"/iRIS/Pan_targ"));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisherP,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),"/iRIS/Pan_Curr"));


  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisherT,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),"/iRIS/Tilt_Curr"));

  // create timer,
  const unsigned int timer_timeout = 100;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    Pos_callback));


    
  // create executor
  RCCHECK(rclc_executor_init(&executorT, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executorT, &subscriberT, &msg_tilt, &Tilt_callback, ON_NEW_DATA));

  RCCHECK(rclc_executor_init(&executorP, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executorP, &subscriberP, &msg_pan, &Pan_callback, ON_NEW_DATA));

  RCCHECK(rclc_executor_init(&executorCP, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executorCP, &timer));

  RCCHECK(rclc_executor_init(&executorCT, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executorCT, &timer));


  PID_UpdateTimer.begin(Update_PID, 5); 
}

void loop() {

  RCCHECK(rclc_executor_spin_some(&executorT, RCL_MS_TO_NS(100)));
  RCCHECK(rclc_executor_spin_some(&executorP, RCL_MS_TO_NS(100)));
  RCSOFTCHECK(rclc_executor_spin_some(&executorCP, RCL_MS_TO_NS(100)));
  RCSOFTCHECK(rclc_executor_spin_some(&executorCT, RCL_MS_TO_NS(100)));

}
