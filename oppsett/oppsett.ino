#include <NewPing.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#define QTR_THRESHOLD  300 
#define NUM_SENSORS 6

const int echoPin = 5;
const int triggerPin = 6;
const int maxDistance = 75;
const int MAX_SPEED = 400;

unsigned int sensor_values[NUM_SENSORS];

unsigned int currentProgram;

bool snurUtAvHvitKant = false;
bool snurMotVenstre = false;

NewPing sonar(triggerPin, echoPin, maxDistance);
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors; //(QTR_NO_EMITTER_PIN);

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
    // Avstanden er 0, la roboten snu
    motors.setSpeeds(-225, 225);
  }else{
    // Avstanden er ikke 0, roboten finnes -> finn den
    if(isInBlack()){
      // Vi er i svart område, kjør mot roboten
      snurUtAvHvitKant = false;
      motors.setSpeeds(400,400);
    }else{
      // Vi er på hvit kant
      if(snurUtAvHvitKant){
        // Vi holder på å komme oss bort fra den hvite kanten, fortsett å snu
        continueTurnOutOfBorder();
      }else{
        // Vi har akkurat kommet til hvit kant, finn ut hvilken vei vi vil snu og start snuingen
        turnOutOfBorder(); 
      }
    }
  }
  delay(50);
}
void continueTurnOutOfBorder(){
  // Fortsett å snu i retningen vi vil
  if(snurMotVenstre){
    motors.setSpeeds(-250, 250);
  }else{
    motors.setSpeeds(250, -250);
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
