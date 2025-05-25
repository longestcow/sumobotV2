#include <Adafruit_VL53L0X.h>

int line1S = A1, line2S = A6;
int startPin = 4;

#define distFS A3
#define distRS A2

int m1A=6, m1B=5, m2A=10, m2B=9; 

int start = 0;
int state = 0;
//code logic
//start sequences
//0 - move back
//1 - turn left and ram

//states
//0 - search - turn with emphasis on right due to right sensor
//1 - honing - try to straighten yourself with respect to opp
//2 - locked in - ram when close enough)
//3 - bounce (line detection) - always happening, highest priority if triggered
// on start, after turn left, start locked in but look out for edge

//value holders
Adafruit_VL53L0X distF = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure1;

int distForward = 0;
int lineL = 0, lineR = 0; 
bool started = false;

int currentSpeed = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(startPin, INPUT_PULLUP);
  pinMode(distFS, OUTPUT);
  pinMode(distRS, OUTPUT);
  pinMode(m1A, OUTPUT);
  pinMode(m1B, OUTPUT);
  pinMode(m2A, OUTPUT);
  pinMode(m2B, OUTPUT);

  digitalWrite(distFS, LOW);
  digitalWrite(distRS, LOW);
  delay(10);

  setAddresses();
  delay(1000);
  //start sequence choice
  int seq1 = 0, seq2 = 0;
  for(int i = 0; i<10; i++) { // close range = move back and turn right, med range = turn left and ram, far range = move back
    getDistances();
    Serial.println(distForward);
    if(distForward<100)
      seq2++;
    else
      seq1++;

    delay(100);
  }
  if(seq1>seq2)
    start = 0;
  else
    start = 1;

    
  Serial.print("choice made ");
  Serial.println(start);
}

void loop() {
  while(!started){
    if(digitalRead(startPin)==LOW){
      Serial.println("button pressed");
      started=true;
      for(int i = 5; i>0; i--){
        Serial.println(i);
        delay(1000);
      }     
      delay(50);

      if(start==0){
        moveBackward(180, 250);
        stopMoving(0);
      }
      else if(start==1){
        turnRight(200, 80); // 90 deg turn
        moveForward(225, 0);
        state=1;
      }
    }
  }

  
  getDistances();
  Serial.println(distForward);
  lineL = analogRead(line1S); 
  lineR = analogRead(line2S); 

  if(lineL<200 || lineR<200){ // move back straight
    moveBackward(150, 200);
    turnLeft(200, 1500); //turn around
    stopMoving(0);

  }
  // looking at distance measures, decide between state, and modify motor movement

  if(state==0) { // searching
        if(distForward<800){ // start honing
            state=1;
        }
        else { // keep searching
          if(start==0)
            turnLeft(50, 0);
          else
            turnRight(50,0);
        }
    }
    
    if(state==1) { // approach
        // if front distance is close enough, switch to ram state
        if(distForward<250)
            state=2;
        
        else if(distForward<800) {
            moveForward(80,0);
        }
        else
            state=0;
        // if nothing in view, switch back to search
    }
    
    if(state==2) {
        //if closer, become faster
        //if nothing in view, switch back to search
        if(distForward<100)
            moveForward(230, 0);
        else if(distForward<250)
            moveForward(140, 0);
        else 
            state=0; // switch to honing
    }

}

void turnRight(int spe, int ms) {
  moveMotors(-spe, spe, ms);
}

void turnLeft(int spe, int ms) {
  moveMotors(spe, -spe, ms);
}

void moveForward(int spe, int ms) {
  moveMotors(spe, spe, ms);
}

void moveBackward(int spe, int ms) {
  moveMotors(-spe,-spe,ms);
}

void stopMoving(int ms){
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
  digitalWrite(distRS,LOW);
  delay(10);
  
  if(!distF.begin()) {
    Serial.println(F("Failed to boot forward VL53L0X"));
    while(1);
  }
  delay(10);

  Serial.println("sensors started");
}

void getDistances(){
  distF.rangingTest(&measure1, false); 
  if(measure1.RangeStatus!=4)
    distForward = measure1.RangeMilliMeter;
  else 
    distForward = 9999;

}
