#include <NewPing.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#define QTR_THRESHOLD  300

const int echoPin = 5;
const int triggerPin = 6;
const int maxDistance = 75;

unsigned int sensor_values[6]; // There are a total of 6 sensors

bool snurUtAvHvitKant = false;
bool snurMotVenstre = false;

int forwardSpeed = 400;

NewPing sonar(triggerPin, echoPin, maxDistance);
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors; //(QTR_NO_EMITTER_PIN);

void setup() {
  
  Serial.begin(9600);
  // Setup reflectance sensor
  reflectanceSensors.init();
  
  button.waitForButton();
  
}

void loop() {

  // TODO: Update forwardSpeed on bluetooth response
  
  simpleFollowBot();
}

void simpleFollowBot(){
  float distance = readDistance(); // Get disance from arduino sensor
  
  if(distance == 0){
    // Avstanden er 0, la roboten snu
    motors.setSpeeds(-225, 225);
  }else if(isInBlack()){
      // Vi er i svart område, kjør mot roboten
      snurUtAvHvitKant = false;
      motors.setSpeeds(forwardSpeed,forwardSpeed);
  }else{
    // Vi er på hvit kant, det vi ser er utenfor banen
    if(snurUtAvHvitKant){
      // Vi holder på å komme oss bort fra den hvite kanten, fortsett å snu
      continueTurnOutOfBorder();
    }else{
      // Vi har akkurat kommet til hvit kant, finn ut hvilken vei vi vil snu og start snuingen
      turnOutOfBorder(); 
    }
  }
    
  delay(50); // Kort delay for å la sensoren finne ny maling
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
  // Vi har truffet en kan og skal kjøre ut.
  // Først sjekk om venstre er sensor er sort. Dersom den leser sort skal roboten snu mot venstre
  snurMotVenstre = sensor_values[0] > QTR_THRESHOLD;

  // Start med å rygge i 0.25s
  motors.setSpeeds(-300, -300);
  delay(250);
  
  continueTurnOutOfBorder(); // Snu i riktig retning (definert av snurmotVenstre)

  // Sett variabel for å snu ut av hvit kant, så den ikke begynner å rotere andre veien om den nå ser en robot.
  snurUtAvHvitKant = true;
}

bool isInBlack(){
  // Returns true if robot sensor 0 and 4 registeres black area (sensor 5 is defect and not in use)
  return (sensor_values[0] > QTR_THRESHOLD) && (sensor_values[4] > QTR_THRESHOLD);
}

float readDistance(){
  reflectanceSensors.read(sensor_values);
  // Gjør ett ping, og beregn avstanden
  unsigned int time = sonar.ping();
  float distance = sonar.convert_cm(time);
  return distance;
}

