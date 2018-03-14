/*
Drive forward and turn left or right when border is detected
  - Only reflectionsensor 0 and 5 are used.
*/
#include <Servo.h>
#include <NewPing.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  300 // 

// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  600 // ms
#define TURN_DURATION     600 // ms
 
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors;

Servo barrier;
const int echoPin = 2, triggerPin = 1;
const float pi = 3.14;
int rotation = 0;
int rotVelocity = 5;
bool rotate360 = false;
int degDest = 0;
int prevDest = -1;
 
void setup()
{
  Serial.begin(9600);
  sensors.init();
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  barrier.attach(3);
  barrier.write(0);
  button.waitForButton();
}

void loop()
{
  sensors.read(sensor_values);
  
  NewPing sonar(triggerPin, echoPin, 100);//Try to keep max distance (third parameter) as short as possible for optimization.
  // Get the time for one ping trip
  unsigned int time = sonar.ping();
  // Calculate how far this time represent
  float dist = sonar.convert_cm(time);
  
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
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
  else if(!rotate360){
    //
    if(barrier.read() < 5){
      rotVelocity = 5;
      if(prevDest == degDest){
        rotate360 = true;
        Serial.println(rotate360);
      }
      prevDest = degDest;
    }
    else if(175 < barrier.read()){
      rotVelocity = -5;
      if(prevDest == degDest){
        rotate360 = true;
        Serial.println(rotate360);
      }
      prevDest = degDest;
    }
    else{
      //Drive in an arc towards target.
      float rad = (prevDest * pi) / 180;
      float turnStrength = sin(rad);
      if(prevDest < 90){
        motors.setSpeeds(FORWARD_SPEED * turnStrength, FORWARD_SPEED);
      }
      else{
        motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED * turnStrength);
      }
    }
    rotation += rotVelocity;
    barrier.write(rotation);
    delay(10);
  }
  else{
    //Otherwise, scan 360 by turning the car.
    barrier.write(90);

    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);

    if(0 < dist && dist < 75){
      rotate360 = false;
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
  
  if(0 < dist && dist < 15){
    degDest = barrier.read();
  }

  //Serial.println(distance);
}
