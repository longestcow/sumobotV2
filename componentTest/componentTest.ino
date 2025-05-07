#include <Adafruit_VL53L0X.h>


int line1S = A1, line2S = A6;
int startPin = 4;
#define distFS A3
#define distRS A2
#define distPS A0

int m1A=6, m1B=5, m2A=10, m2B=9;
int pastFrontDistances[5] = {9999,9999,9999,9999,9999};
int pastRightDistances[5] = {9999,9999,9999,9999,9999};
long pastDistanceTime = 0;

//code logic
//start sequences
//0 - move back
//1 - turn left and ram
//2 - move back and turn right and ram (punishing those who do 1)

//states
//0 - search - turn with emphasis on right due to right sensor
//1 - honing - try to straighten yourself with respect to opp
//2 - locked in - ram when close enough)
//3 - bounce (line detection) - always happening, highest priority if triggered
// on start, after turn left, start locked in but look out for edge

int state = 0, startSequence = 0;

//value holders
Adafruit_VL53L0X distF = Adafruit_VL53L0X();
Adafruit_VL53L0X distR = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

int distForward = 0, distRight = 0;
int lineL = 0, lineR = 0; 
bool started = false;

int currentSpeed = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(startPin, INPUT_PULLUP);
  pinMode(distFS, OUTPUT);
  pinMode(distPS, OUTPUT);
  pinMode(distRS, OUTPUT);
  pinMode(m1A, OUTPUT);
  pinMode(m1B, OUTPUT);
  pinMode(m2A, OUTPUT);
  pinMode(m2B, OUTPUT);

  digitalWrite(distFS, LOW);
  digitalWrite(distPS, LOW);
  digitalWrite(distRS, LOW);
  delay(10);

  setAddresses();
  delay(1000);
  //start sequence choice
  int seq1 = 0, seq2 = 0;
  for(int i = 0; i<10; i++) {
    getDistances();
    if(distForward<distRight)
      seq1++;
    else 
      seq2++;
    delay(100);
  }
  
  if(seq1+seq2 < 5)
    startSequence = 0;
  else if(seq1<seq2)
    startSequence = 1;
  else
    startSequence = 2;
    
  Serial.print("choice made ");
  Serial.println(startSequence);
  pastDistanceTime = millis();
}

void loop() {
  while(!started){
    if(digitalRead(startPin)==LOW){
      Serial.println("button pressed");
      if(startSequence==0) 
        Serial.println("move start");
      else if(startSequence==1)
        Serial.println("ram start");
      else
        Serial.println("move ram start");
      started=true;
      for(int i = 5; i>0; i--){
        Serial.println(i);
        delay(1000);
      }     

      if(startSequence==1){ // ram
        turnLeft(200, 500); // 90 deg turn
        moveForward(240, 3000);
      }
      else if(startSequence==0){ // move away
        moveBackward(240, 1000);
      }
      else { // move ram
          moveBackward(240,1000);
          stopMoving(1000);
          moveMotors(200, 140, 1000);
      }
    }
  }

  
    
  lineL = analogRead(line1S); 
  lineR = analogRead(line2S); 
  // priority - state 3
  if(lineL>400 && lineR>400){ // move back straight
    moveBackward(180, 1000);
    turnRight(180, 500); //turn around
  }
  else if(lineL>400) {
    moveMotors(-180, -140, 1000);
    turnRight(180, 500); //turn around
  }
  else if(lineR>400) {
    moveMotors(-140, -180, 1000);
    turnRight(180, 500); // turn around
  }
  // at this point, line edge detection is handled. no delays from this point onward

  // looking at distance measures, decide between state, and modify motor movement
    getDistances();
    if(state==0) { // searching
        if(distRight<100 || distForward<100){ // start honing
            state=1;
        }
        else { // keep searching
            turnRight(130, 0);
        }
    }
    
    if(state==1) { // honing
        // make use of past distances while slowly approaching
        // if front distance is close enough, switch to ram state
        if(distForward<50 && distRight>80)
            state=2;
        
        else if(distForward<100 || distRight<100) {
            //check past distances and distRight to pivot
            // move according to pivot
        }
        
        else
            state=0;
        // if nothing in view, switch back to search
    }
    
    if(state==2) {
        //if closer, become faster
        //if nothing in view, switch back to search
        if(distForward<20)
            moveForward(240, 0);
        else if(distForward<50)
            moveForward(200, 0);
        else 
            state=1; // switch to honing
    }
    
    
    if(pastDistanceTime<millis()){ // update past distances
        pastDistanceTime=millis()+100;
        for(int i = 4; i>0; i++){
            pastFrontDistances[i]=pastFrontDistances[i-1];
            pastRightDistances[i]=pastRightDistances[i-1];
        }
        pastFrontDistances[0]=distForward;
        pastRightDistances[0]=distRight;
    }
}

void turnLeft(int spe, int ms) {
  Serial.println("left");
  moveMotors(-spe, spe, ms);
}

void turnRight(int spe, int ms) {
  Serial.println("right");
  moveMotors(spe, -spe, ms);
}

void moveForward(int spe, int ms) {
  Serial.println("forward");
  moveMotors(spe, spe, ms);
}

void moveBackward(int spe, int ms) {
  Serial.println("backward");
  moveMotors(-spe,-spe,ms);
}

void stopMoving(int ms){
  Serial.println("stop");
  moveMotors(0,0,ms);
}

void moveMotors(int m1, int m2, int ms){
  if(m1<0){
    analogWrite(m1A, LOW);
    analogWrite(m1B, m1*-1);
  }
  else {
    analogWrite(m1A, m1);
    analogWrite(m1B, LOW);
  }
  if(m2<0){
    analogWrite(m2A, LOW);
    analogWrite(m2B, m2*-1);
  }
  else {
    analogWrite(m2A, m2);
    analogWrite(m2B, LOW);
  }
  delay(ms);
}

void setAddresses(){
  digitalWrite(distFS,LOW);
  digitalWrite(distRS,LOW);
  delay(10);
  
  digitalWrite(distFS,HIGH);
  digitalWrite(distRS,HIGH);
  delay(10);

  digitalWrite(distFS,HIGH);
  digitalWrite(distRS,LOW);
  delay(10);
  
  if(!distF.begin(0x30)) {
    Serial.println(F("Failed to boot forward VL53L0X"));
    while(1);
  }
  delay(10);

  digitalWrite(distRS, HIGH);
  delay(10);

  if(!distR.begin(0x31)) {
    Serial.println(F("Failed to boot right VL53L0X"));
    while(1);
  }
  delay(10);

  Serial.println("sensors started");
}

void getDistances(){
  distF.rangingTest(&measure1, false); 
  distR.rangingTest(&measure2, false); 
  if(measure1.RangeStatus!=4)
    distForward = measure1.RangeMilliMeter;
  else 
    distForward = 9999;
  if(measure2.RangeStatus!=4)
    distRight = measure2.RangeMilliMeter;
  else
    distRight = 9999;
}
