#include <Servo.h>

Servo barrier;
const int echoPin = 11, triggerPin = 10;
int rotation = 0;
int rotVelocity = 5;
bool rotate360 = false;
int degDest = 0;
int prevDest = -1;

float getDistance() {
  //Make echo call.
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  //Determine echo return duration.
  long duration = pulseIn(echoPin, HIGH);
  //Calculate distance.
  float distance = duration * 0.00017;
  return distance;
}

void setup() {
  Serial.begin(9600);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  barrier.attach(8);
  barrier.write(0);
}

void loop() {
  if(!rotate360){
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
    rotation += rotVelocity;
    barrier.write(rotation);
    delay(10);
  }
  else{
    barrier.write(90);
    //Turn the car.
  }
  float dist = getDistance();
  //Serial.println(dist);
  if(dist < 0.2){
    degDest = barrier.read();
  }
}
