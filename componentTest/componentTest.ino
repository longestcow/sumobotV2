#include <Wire.h>
#include <VL53L0X.h>


//pins
int line1S = A1, line2S = A6;
int startPin = 4;
int distLS = A0, distRS = A2;
int m1A=9, m1B=10, m2A=11, m2B=12; // d12 is not pwm, it has to be a direction pin
//https://github.com/CytronTechnologies/CytronMotorDriver/blob/master/examples/PWM_PWM_DUAL/PWM_PWM_DUAL.ino
//https://github.com/pololu/vl53l0x-arduino/blob/9f3773cb48d4e4e844d689cfc529a06f96d1d264/examples/Continuous/Continuous.ino

//value holders
VL53L0X distL, distF, distR;
int distLeft = 0, distForward = 0, distRight = 0;
int line1, line2;
bool started = false;


//possibly use vl53l 1 x



void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(startPin, INPUT_PULLUP);
  pinMode(distLS, OUTPUT);
//  pinMode(distFS, OUTPUT);
  pinMode(distRS, OUTPUT);

  digitalWrite(distLS, LOW);
//  digitalWrite(distFS, LOW);
  digitalWrite(distRS, LOW);
  delay(50);

  digitalWrite(distLS, HIGH);
  delay(20);
//  if (!distL.init()) {
//    Serial.println("distL init didnt work");
//    while (1) {}
//  }
  distL.init();
//  distL.setAddress(0x30);
//  
////  digitalWrite(distFS, HIGH);
////  delay(20);
////  if (!distF.init()) {
////    Serial.println("distF init didnt work");
////    while (1) {}
////  }
////  distF.setAddress(0x31);
//  
//  digitalWrite(distRS, HIGH);
//  delay(20);
//  if (!distR.init()) {
//    Serial.println("distR init didnt work");
//    while (1) {}
//  }
//  distR.setAddress(0x32);
//
//  distL.startContinuous();
////  distF.startContinuous();
//  distR.startContinuous();

  Serial.println("sensors started");
}

void loop() {
  while(!started){
    if(digitalRead(startPin)==LOW){
      Serial.println("button pressed");
      started=true;
    }
  }
  //5 second delay

//  distLeft = distL.readRangeContinuousMillimeters();
//  if (distL.timeoutOccurred()) Serial.println("timed out distL");
//
////  distForward = distF.readRangeContinuousMillimeters();
////  if (distF.timeoutOccurred()) Serial.println("timed out distF");
//
//  distRight = distR.readRangeContinuousMillimeters();
//  if (distR.timeoutOccurred()) Serial.println("timed out distR");
  
  
  line1 = analogRead(line1S);
  line2 = analogRead(line2S);
  Serial.print("left line: ");
  Serial.println(line1);
  Serial.print("right line: ");
  Serial.println(line2);
  Serial.print("left dist: ");
  Serial.println(distLeft);
//  Serial.print("forward dist: ");
//  Serial.println(distForward);
  Serial.print("right dist: ");
  Serial.println(distRight);
  Serial.println("-------------------");
  delay(1000);
  
}
