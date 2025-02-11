#include <Servo.h>

const int maxDegreeToLeftShoulder = 40;
const int maxDegreeToRightShoulder = 90;
const int centeredDegree = 65;
const boolean headturndebug = false;

Servo headRotationServo;
int headRotationServoPosition = 0;

void setupHeadTurn() {
  // put your setup code here, to run once:
  headRotationServo.attach(headRotationServoControlPin);
}

void loopHeadTurn() {

//testTurn();

  if(headturnActive) {
    if(state == panningMode) controlledWrite(maxDegreeToLeftShoulder,headTurnMillisForPanningMode, panningMode);
    if(state == panningMode) controlledWrite(maxDegreeToRightShoulder,headTurnMillisForPanningMode, panningMode);
  
    if(state == risingMode) controlledWrite(centeredDegree, headTurnMillisForRisingingMode, risingMode);
  }
}

/*
 * degree = destination degree for servo to reach
 * duration = how fast to get the "degree" in millis
 * whileInState = allows this method to exit if state engine changes state
 */
void controlledWrite(int degree, int duration, int whileInState) {
  headRotationServoPosition = headRotationServo.read();
  int spread = abs(degree - headRotationServoPosition);
  //only perform the motion if not already around the spot
  if(spread > 5) {
    int delayMs = duration/spread;
    if(delayMs > 200) delayMs = 200;
    int increment = spread/delayMs;
    if(increment < 1) increment = 1;
    if(headturndebug) log("HeadTurn: degree: " + String(degree) + " loc: " + String(headRotationServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));
    if(degree > headRotationServoPosition)  {
      while(headRotationServoPosition < degree && state == whileInState) {
        if(headturndebug) log("HeadTurn: loc: " + String(headRotationServoPosition) + " moving " + String(increment) + " degress, delaying " + String(delayMs) + "ms");
        headRotationServoPosition = headRotationServoPosition + increment;
        headRotationServo.write(headRotationServoPosition);
        delay(delayMs);
  //      headRotationServoPosition = headRotationServo.read();
      } 
    } else {
      while(headRotationServoPosition > degree && state == whileInState) {
        if(headturndebug) log("HeadTurn: loc: " + String(headRotationServoPosition) + " moving " + String(-1*increment) + " degress, delaying " + String(delayMs) + "ms");
        headRotationServoPosition = headRotationServoPosition - increment;
        headRotationServo.write(headRotationServoPosition);
        delay(delayMs);
  //      headRotationServoPosition = headRotationServo.read();
      } 
    }
  }
}

void testTurn() {
    digitalWrite(GREEN_LED, HIGH);
  headRotationServoPosition = headRotationServo.read();
  delay(200);
  headRotationServo.write(headRotationServoPosition+4);
  delay(200);
  headRotationServoPosition = headRotationServo.read();
  delay(200);
  headRotationServo.write(headRotationServoPosition+4);
  delay(200);
  headRotationServoPosition = headRotationServo.read();
  delay(200);
  headRotationServo.write(headRotationServoPosition-4);
  delay(200);
  headRotationServoPosition = headRotationServo.read();
  delay(200);
  headRotationServo.write(headRotationServoPosition-4);
    digitalWrite(GREEN_LED, LOW);
  delay(200);  
}
