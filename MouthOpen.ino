#include <Servo.h>

Servo mouthOpenServo;
int mouthOpenServoPosition = 0;

const boolean mouthOpenDebug = false;

int mouthOpenDegree = 160;
int mouthClosedDegree = 120;
int mouthPantDuration = 2000;
int mouthShutDuration = 3000;
 
// the setup routine runs once when you press reset:
void setupTaskMouthOpen() {       
  mouthOpenServo.attach(mouthOpenServoControlPin);
}

// the loop routine runs over and over again forever:
void loopTaskMouthOpen() {
/*
while(true) {
  log("mouthOpenServo.write(170)");
  mouthOpenServo.write(170);
  delay(1000);
    log("mouthOpenServo.write(125)");
    mouthOpenServo.write(120);
  delay(1000);
}
*/
  if(mouthActive) {
    if(state == panningMode) mouthOpencontrolledWrite(mouthOpenDegree,mouthPantDuration, panningMode);
    if(state == panningMode) mouthOpencontrolledWrite(mouthClosedDegree,mouthPantDuration, panningMode);
    
  
    if(state == risingMode) {
      mouthOpencontrolledWrite(mouthClosedDegree,3000, risingMode);
      while(state == risingMode) {
        delay(100);
        if ( mouthOpenDebug ) log("MouthClosed delaying...");
      }
    }
  
  }
}

void mouthOpencontrolledWrite(int degree, int duration, int whileInState) {
  int loc = mouthOpenServo.read();
  int spread = abs(degree - loc);
  int delayMs = duration/spread;
  int increment = spread/delayMs;
  if(increment < 2) increment = 2;
  if ( mouthOpenDebug ) log("MouthOpen: degree: " + String(degree) + " loc: " + String(mouthOpenServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));
  if(spread >1) { 
    if(degree < loc)  {
      while(loc > degree +2 && state == whileInState) {
        loc = loc - increment;
        mouthOpenServo.write(loc);
        delay(delayMs);
      }  
    } else {
      while(loc < degree-2  && state == whileInState) {
        loc = loc + increment;
        mouthOpenServo.write(loc);
        delay(delayMs);
      } 
    }
  }
}
