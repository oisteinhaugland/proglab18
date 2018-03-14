#include  <SoftwareSerial.h> 
#include  <PLabBTSerial.h>

#define txPin 7
#define rxPin 5

char BTName[] = "marcuskb";
char ATCommand[] = "AT+NAMEPLab_";
PLabBTSerial btSerial(txPin, rxPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btSerial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Set name");
  Serial.println(BTName);

  btSerial.write(ATCommand);
  btSerial.write(BTName);
  btSerial.write(0x0D);  btSerial.write(0x0A);
  delay(1000);

}
