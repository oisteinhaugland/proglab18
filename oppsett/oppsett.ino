#include <NewPing.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#define QTR_THRESHOLD  300 // this might need to be tuned for different
                            //lighting conditions, surfaces, etc
#define SPEED     100 

const int echoPin = 5;
const int triggerPin = 6;
const int maxDistance = 75;
const int MAX_SPEED = 400;
NewPing sonar(triggerPin, echoPin, maxDistance);
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
int currentProgram;
ZumoReflectanceSensorArray reflectanceSensors; //(QTR_NO_EMITTER_PIN);

bool snurUtAvHvitKant = false;
bool snurMotVenstre = false;
void setup() {
  Serial.begin(9600);
  reflectanceSensors.init();
  button.waitForButton();
  currentProgram = 0;
}
bool isInBlack();
void turnOutOfBorder(); 
void loop() {
  switch(currentProgram){
    case 0:
    default:
      simpleFollowBot();
      break;
  }
}

void simpleFollowBot(){
  
  reflectanceSensors.read(sensor_values);
  // Gjør ett ping, og beregn avstanden
  // Do ping and calculate distance
  unsigned int time = sonar.ping();
  float distance = sonar.convert_cm(time);
  if(distance == 0){
    Serial.println("is 0");
    motors.setSpeeds(-225, 225);
    // Rotere
  }else{
    Serial.println(distance);
    if(isInBlack()){
      // Kjør fram
      snurUtAvHvitKant = false;
      motors.setSpeeds(400,400);
    }else{
      if(snurUtAvHvitKant){
        continueTurnOutOfBorder();
      }else{
        turnOutOfBorder(); 
      }
      
    }
  }
  delay(50);
}
void continueTurnOutOfBorder(){
  if(snurUtAvHvitKant){
    // Har allerede begynt å snu
    if(snurMotVenstre){
      motors.setSpeeds(-250, 250);
    }else{
      motors.setSpeeds(250, -250);
    }
  }
}
void turnOutOfBorder(){
  motors.setSpeeds(-300, -300);
  delay(250);
  if(sensor_values[0] < QTR_THRESHOLD){
    motors.setSpeeds(250, -250);
    // Snu mot høyre
    snurMotVenstre = false;
  }else{
    motors.setSpeeds(-250, 250);
    // Snu mot venstre
    snurMotVenstre = true;
  }
  snurUtAvHvitKant = true;

}
bool isInBlack(){
  return (sensor_values[0] > QTR_THRESHOLD) && (sensor_values[4] > QTR_THRESHOLD);
}
