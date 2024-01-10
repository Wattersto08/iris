#include <Arduino.h>

#define TWO_PI 6.283185307179586476925286766559

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


unsigned long limswX_interrupt_time = 0;
unsigned long limswX_interrupt_time_prev = 0;

unsigned long limswY_interrupt_time = 0;
unsigned long limswY_interrupt_time_prev = 0;

unsigned long limsw_debounce_time = 300;

long X_pos = 0;
int X_Counts_per_rev = 2048;

long Y_pos = 0;
int Y_Counts_per_rev = 2048;

void X_limsw_interrupt();
void X_encoder_interrupt();

void Y_limsw_interrupt();
void Y_encoder_interrupt();

void Set_motor_power(bool Mselect, int pwr);
void Disable_motor();

float enc_count_to_rad(float counts_per_rev, float curr_count){
  return ((curr_count/counts_per_rev)*TWO_PI);
}

void setup() {
  
  Serial.begin(9600);

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

  Serial.println("\nHardware Tests Beginning\n");

  Serial.println("\nSetting Y Axis motor Forward\n");
  Set_motor_power(1,-256);
  delay(500);
  for(int i = -256; i < 256;i++){
    Set_motor_power(1,i);
    delay(5);  
  }
  Set_motor_power(1,256);
  delay(500);

  Serial.println("\nDisbale Motors\n");
  //Disable_motor();  
}

void loop() {
  Serial.println(enc_count_to_rad(Y_Counts_per_rev,Y_pos));
  delay(20);
}

void X_limsw_interrupt() {
  limswX_interrupt_time = millis();
  if((limswX_interrupt_time - limswX_interrupt_time_prev)>limsw_debounce_time){
    delay(100);
    if(digitalRead(Xlimsw_pin)== HIGH){
      X_pos = 0;
      Serial.println(" - X homed -");
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
      Serial.println(" - Y homed -");
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