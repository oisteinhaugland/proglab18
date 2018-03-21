/*
 */

#include <SoftwareSerial.h>
#include <PLabBTSerial.h>

const int txPin = 2; // Connected to tx on bt unit
const int rxPin = 3; // Connected to rx on bt unit

PLabBTSerial btSerial(txPin, rxPin);

// Always include this method.
// It reads from the BT port and calls BTSerialMessageReceived.
// 
char msg[100];
void updateBTSerial() {
  int availableCount = btSerial.available();
  if (availableCount > 0) {
  //  char *msg = new char[availableCount];
    btSerial.read(msg, availableCount);
    char *divided = strchr(msg,',');
    int msgValue = 0;
    if (divided != 0) {
       divided[0] = 0; divided++;
       String str(divided);
       msgValue = str.toInt();
    };
    String msgString(msg);
    BTSerialMessageReceived(msgString,msgValue);   
  }
}

void setup()
{  
  btSerial.begin(9600); // Open serial communication to Bluetooth unit

  Serial.begin(9600);   // Open serial communication to Serial Monitor
  Serial.println("Hallo world!");
}

void loop() {
   updateBTSerial();
}

void BTSerialMessageReceived(String msgString,int msgValue) {
  Serial.print("Message:"); Serial.print(msgString); 
  Serial.print(", Value:"); Serial.println(msgValue); 
}


