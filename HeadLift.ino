#include <Servo.h>

/*
 * 0 degrees = head looking up from body (to sky when standing, forward when crounching)
 * 179 degrees = head looking for when standing, down when crouching
 */

const int headDown = 100; //degrees
const int headUp = 150; //degrees
const boolean headliftdebug = false;

Servo headLiftServo;
int headLiftServoPosition = 0;
  
// the setup routine runs once when you press reset:
void setupHeadLift() {                
  headLiftServo.attach(headLiftServoControlPin);
  headLiftServo.write(90);
}

// the loop routine runs over and over again forever:
void loopHeadLift() {

  //nod();
  
  //flag to shut-off head lifting effect
  if(headliftActive) {
    if(headliftdebug) log("HeadLift: loc: " + String(headLiftServoPosition) + " mode: " +  state);
//    if(state == panningMode )   speedControlledHeadLiftWrite(headUp, 6000, panningMode);
    if(state == panningMode && headLiftServoPosition < headUp)   speedControlledHeadLiftWrite(headUp, 6000, panningMode);
    if(state == risingMode && headLiftServoPosition > headDown)  speedControlledHeadLiftWrite(headDown, headLiftMillisForRisingMode, risingMode);
    if(state == howlingMode && headLiftServoPosition < headUp)   speedControlledHeadLiftWrite(headUp, headLiftMillisForHowlingMode, howlingMode);
    if(state == loweringMode && headLiftServoPosition < headUp)  speedControlledHeadLiftWrite(headUp, headLiftMillisForLoweringMode, loweringMode);
  }
  
}

void speedControlledHeadLiftWrite(int degree, int duration, int whileInState) {
  headLiftServoPosition = headLiftServo.read();
  int spread = abs(degree - headLiftServoPosition);
  
  if(spread <1) return; // if there is nowhere to move, exit function
  
  int delayMs = duration/spread;
  if(delayMs > 200) delayMs = 200;
  int increment = spread/delayMs;
  if(increment < 1) increment = 1;
  if ( headliftdebug ) log("Headlift: degree: " + String(degree) + " loc: " + String(headLiftServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));

  if(degree < headLiftServoPosition)  {
    while(headLiftServoPosition > degree && state == whileInState) {
      if ( headliftdebug ) log("Headlift down loop: degree: " + String(degree) + " loc: " + String(headLiftServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));
      headLiftServoPosition = headLiftServoPosition - increment;
      headLiftServo.write( headLiftServoPosition );
      delay(delayMs);
    }  
  } else {
    while(headLiftServoPosition < degree && state == whileInState) {
      if ( headliftdebug ) log("Headlift up loop: degree: " + String(degree) + " loc: " + String(headLiftServoPosition) + " spread: " + String(spread) + " delayMs: " + String(delayMs) + " increment: " + String(increment));
      headLiftServoPosition = headLiftServoPosition + increment;
      headLiftServo.write( headLiftServoPosition );
      delay(delayMs);
    } 
  }
}


void nod() {
  log("nod");
    digitalWrite(DEBUG_LED, HIGH);
  headLiftServoPosition = headLiftServo.read();
  if(headLiftServoPosition ==92) digitalWrite(HOWLING_LED, HIGH);
  delay(200);
  headLiftServo.write(headLiftServoPosition+4);
  delay(200);
  headLiftServoPosition = headLiftServo.read();
  if(headLiftServoPosition ==96) digitalWrite(HOWLING_LED, LOW);
  delay(200);
  headLiftServo.write(headLiftServoPosition+4);
  delay(200);
  headLiftServoPosition = headLiftServo.read();
  delay(200);
  headLiftServo.write(headLiftServoPosition-4);
  delay(200);
  headLiftServoPosition = headLiftServo.read();
  delay(200);
  headLiftServo.write(headLiftServoPosition-4);
    digitalWrite(DEBUG_LED, LOW);
  delay(200);  
}
