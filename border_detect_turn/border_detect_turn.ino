/*
Drive forward and turn left or right when border is detected
  - Only reflectionsensor 0 and 5 are used.
*/
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
 
#define LED 13
 
// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  300 // 
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     0 // 0 is stopped, 400 is full speed
#define TURN_SPEED        0
#define FORWARD_SPEED     0
#define REVERSE_DURATION  0 // ms
#define TURN_DURATION     0 // ms
 
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

const int triggerPin = 3;
const int echoPin = 2;
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors;


 
void setup()
{
   Serial.begin(9600);
   sensors.init();
   button.waitForButton();
   pinMode(triggerPin, OUTPUT);
   pinMode(echoPin, INPUT);
}

void loop()
{
  sensors.read(sensor_values);
  float distance = getDistance();

  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if(distance < 0.20){
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds((TURN_SPEED+50), - (TURN_SPEED - 50));
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  
  else if (sensor_values[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }

  Serial.println(distance);
}


float getDistance(){
   digitalWrite(triggerPin, LOW);
   delayMicroseconds(2);
   digitalWrite(triggerPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(triggerPin, LOW);
   long duration = pulseIn(echoPin, HIGH);
   float distance = duration * 0.00017;
   return distance;
}
