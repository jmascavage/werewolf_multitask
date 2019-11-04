#include <Servo.h>

Servo mouthOpenServo;
int mouthOpenServoPosition = 0;

int mouthOpenDegree = 170;
int mouthClosedDegree = 125;
 
// the setup routine runs once when you press reset:
void setupTaskMouthOpen() {       
  mouthOpenServo.attach(mouthOpenServoControlPin);
}

// the loop routine runs over and over again forever:
void loopTaskMouthOpen() {

  if(mouthActive) {
    if(state == panningMode) mouthOpencontrolledWrite(mouthOpenDegree,7000, panningMode);
    if(state == panningMode) mouthOpencontrolledWrite(mouthClosedDegree,7000, panningMode);
/*    
  
    if(state == risingMode) {
      mouthOpencontrolledWrite(mouthClosedDegree,3000, risingMode);
      while(state == risingMode) {
        delay(100);
        log("MouthClosed delaying...");
      }
    }
  */
  }
}

void mouthOpencontrolledWrite(int degree, int duration, int whileInState) {
  int loc = mouthOpenServo.read();
  int spread = abs(degree - loc);
  int delayMs = duration/spread;
  int increment = spread/delayMs;
  if(increment < 2) increment = 2;
  if ( werewolfdebug ) log("MouthOpen: degree: " + String(degree) + " loc: " + String(mouthOpenServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));
  if(spread >1) { 
    if(degree < loc)  {
digitalWrite(BLUE_LED, HIGH);
      while(loc > degree +2 && state == whileInState) {
        loc = loc - increment;
        mouthOpenServo.write(loc);
        delay(delayMs);
      }  
digitalWrite(BLUE_LED, LOW);
    } else {
digitalWrite(GREEN_LED, HIGH);
      while(loc < degree-2  && state == whileInState) {
        loc = loc + increment;
        mouthOpenServo.write(loc);
        delay(delayMs);
      } 
digitalWrite(GREEN_LED, LOW);
    }
  }
}

