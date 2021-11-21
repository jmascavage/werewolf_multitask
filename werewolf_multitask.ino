//#include <CogLCD.h>
//#include <stdio.h>
//#include <Servo.h>
/*
   Settings for pin layouts, modes, and timings
   All threads (other files) have access to these variables
*/

const int sensorTrigPin               = 12;  // sensorTrigPin tells distance sensor to start/stop a reading
const int sensorEchoPin               = 13;  // sensorEchoPin is where distance sensor provides feedback
const int transistorGatePin           = 5;  // transistorGatePin will flip the transistor to power the pneumatic solenoid valve's 12v power
const int howlingPin                  = 6;
const int breathingPin                = 7;
const int mouthOpenServoControlPin    = 8;
const int headRotationServoControlPin = 9;
const int headLiftServoControlPin     = 11;

const int minTriggerDistance = 10;      // minimum distance, inches,  object must be away in order to trigger
const int maxTriggerDistance = 72;      // maximum distance, inches, object must be away in order to trigger

const int panningMode = 0;
#define PANNING_LED RED_LED
const int risingMode = 1;
#define RISING_LED GREEN_LED
const int howlingMode = 2;
#define HOWLING_LED BLUE_LED
const int loweringMode = 3;
//#define DEBUG_LED YELLOW_LED
#define DEBUG_LED BLUE_LED

const boolean headturnActive = true;
const boolean headliftActive = true;
const boolean mouthActive    = false;
const boolean breathingActive    = true;

int state = 99; //needs to be set to some integer value - NOT one of the constants above - constants can be assigned to it after this point

long counter;  //pure loop counter

const int headTurnMillisForPanningMode   = 10000; //time to go from left-to-right or right-to-left
const int headLiftMillisForRisingMode    = 3000;  //time to get head looking down while rising
const int headTurnMillisForRisingingMode = 3000;  //time to get head pointed forward
const int lengthOfRisingMode             = 6000;  //time spent in rising mode - start to finish
const int headLiftMillisForHowlingMode   = 3000;  //time to raise head to howl
const int lengthOfHowlMillis             = 6000;  //length of howl sound
const int lengthOfHowlingMode            = 6000;  //time spent in howling mode - start to finish
const int headLiftMillisForLoweringMode  = 5000;  //time to lower head while lowering
const int lengthOfLoweringingMode        = 5000;  //time spent in lowering mode - start to finish
const int requiredHitCount               = 2;     //number of matching hits from distance sensor to trigger rise
const int resetMillis                    = 5000;  //time before allowed to re-trigger

// werewolfdebug turns on/off console logging - been having issues with the program locking up 
// when logging to serial port so setting to false avoids this issue
const boolean werewolfdebug = true;
//int ConsoleBps = 9600;
int ConsoleBps = 19200;
//int ConsoleBps = 115200;


void setup()
{
  if(werewolfdebug) Serial.begin(ConsoleBps);
  log("Running setup()...");
  pinMode(DEBUG_LED, OUTPUT); 
  pinMode(PANNING_LED, OUTPUT); 
  pinMode(RISING_LED, OUTPUT); 
  pinMode(HOWLING_LED, OUTPUT); 
  pinMode(howlingPin, OUTPUT);
  digitalWrite(howlingPin, HIGH);
  pinMode(breathingPin, OUTPUT);

  //setup distance sensor pins
  pinMode(sensorTrigPin, OUTPUT);
  log("Sensor Trig pin set to output");
  pinMode(sensorEchoPin, INPUT);
  log("Sensor Echo pin set to input");

  setState(panningMode);
  startBreathing();

  log("Finished setup()...");
}

void loop()
{
  counter++;

  if (isSomeoneClose()) {
    setState(risingMode);

    //raise body by triggering transistor (which powers solenoid)
    log("Raising body...");
    digitalWrite(transistorGatePin, HIGH);     // powers transistor and an LED so we can see it happen
    stopBeathing();

    delay(lengthOfRisingMode);               // wait for length of mode

    setState(howlingMode);  
    delay(headLiftMillisForHowlingMode);
    howl();
    delay(lengthOfHowlingMode);               // wait for length of mode


    setState(loweringMode);
    //lower body
    log("Lowering body...");
    digitalWrite(transistorGatePin, LOW);      // power off transistor (and thus solenoid)
    delay(lengthOfLoweringingMode);               // wait for length of mode

    setState(panningMode);
    startBreathing();

    // delay long enough to allow body to lower and ready for next go...
    delay(resetMillis);
  }
  
  delay(1000);

}

void startBreathing() {
  //start by turning sound off - HIGH = off on adafruit board
  digitalWrite(breathingPin, HIGH);
  if(breathingActive) {
    digitalWrite(breathingPin, LOW);
  }
}
void stopBeathing() {
  digitalWrite(breathingPin, HIGH);
}

void howl() {
  digitalWrite(howlingPin, HIGH);
  digitalWrite(howlingPin, LOW);
  delay(lengthOfHowlMillis);
  digitalWrite(howlingPin, HIGH);

}

boolean isSomeoneClose() {
  int countOfHits = 0;
  long inches = getSensorDistance();
  log("Run 1: " + String(inches) + "in");
  if (inches > minTriggerDistance && inches < maxTriggerDistance) {
    countOfHits = countOfHits + 1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 2: " + String(inches) + "in");
  if (inches > minTriggerDistance && inches < maxTriggerDistance) {
    countOfHits = countOfHits + 1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 3: " + String(inches) + "in");
  if (inches > minTriggerDistance && inches < maxTriggerDistance) {
    countOfHits = countOfHits + 1;
  }

  // print out distance to console
  log("Count of hits: " + String(countOfHits));

  //   delay(2000);

  if (countOfHits >= requiredHitCount ) {
    return true;
  }  else {
    return false;
  }

}

long getSensorDistance()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  long duration = pulseIn(sensorEchoPin, HIGH);

  // convert the time into a distance
  return microsecondsToInches(duration);
}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second). This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

void setState(int newState) {
  state = newState;

  //turn all LEDs off
  digitalWrite(PANNING_LED, LOW);
  digitalWrite(RISING_LED, LOW);
  digitalWrite(HOWLING_LED, LOW);

  if(state == panningMode) {
    digitalWrite(PANNING_LED, HIGH);
  }
  if(state == risingMode) {
    digitalWrite(RISING_LED, HIGH);
  }
  if(state == howlingMode) {
    digitalWrite(HOWLING_LED, HIGH);
  }  
  
  log(state);
}

void log(String logLine)
{
  if ( werewolfdebug ) {
    Serial.println(String(millis()) + ": " + logLine);
  }
}
