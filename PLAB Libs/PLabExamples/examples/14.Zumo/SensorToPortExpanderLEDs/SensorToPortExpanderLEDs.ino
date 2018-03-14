/*
 * SensorToPortExpanderLEDs
 *
 * Leser av verdiene fra Zumos refleksjonssensorer, tolker signalet
 * som et analogt signal og lyser en LED per sensor på en port
 * expander. Lysets intensitet er bestemt av styrken på avlest
 * signal.
 *
 * Kretsen:
 *   1 x Zumo med sensor array
 *   6 x LED
 *   6 x 220 ohm motstand
 *   1 x 10 k ohm motstand
 * Oppkobling:
 * https://www.ntnu.no/wiki/display/plab/9.+Port+Expander
 *
 *
 * Reads values from the Zumo reflection sensors, interprets the
 * signal as an analog signal and shines one LED per sensor. The
 * LEDs should be connected to a port expander. The light intensity
 * is determined by signal strength.
 *
 * Circuit:
 *   1 x Zumo with reflectance senosr array
 *   6 x LED
 *   6 x 220 ohm resistor
 *   1 x 10 k ohm resistor
 * Connecting the circuit:
 * https://www.ntnu.no/wiki/display/plab/9.+Port+Expander
 */

// inttypes holds the uint8_t (unsigned integer, 8 bit) type
#include <inttypes.h>
// Zumo library includes
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
// Port expander includes
#include <Wire.h>
#include <Adafruit_MCP23008.h>

const int addr = 0;  // Port expander address
Adafruit_MCP23008 mcp;  // Port exander proxy

ZumoReflectanceSensorArray sensors; // Sensor array proxy
const unsigned int maxReading = 2000;  // Maximum value that can be read from sensors (defined in library)

unsigned int readings[8];  // Will hold the read sensor values. Must be at least 6 big for reading. Using the same array for writing, it must be at least 8 big


/**
 * Helper function that initialize a MCP23008 port expander with the given address, with all IO ports set as outputs.
 *   peAddr  : Address of the port expander
 *   portExp : The port expander. Passed by reference, no copies are made of the proxy object.
 */
void portExpander_beginAnalog(int peAddr, Adafruit_MCP23008 &portExp) {
  // Start communication with port expander
  portExp.begin(peAddr);
  // Set all pins as outputs
  for (int i = 0; i < 8; ++i) {
    portExp.pinMode(i, OUTPUT);
  }
}

/**
 * Helper function that simulate 1 cycle of PWM written analog output.
 *   values  : Array of values. MUST be AT LEAST 8 elements in array. Values are expected in range 0 - 100
 *   portExp : The port expander. Passed by reference, no copies are made of the proxy object.
 */
void portExpander_analogWrite(unsigned int *values, Adafruit_MCP23008 &portExp) {
  
  // Expected value range: 0 - MAX_VALUE
  const int MAX_VALUE = 100;
  // Step size: Determines resolution. Too high resolution (low step size) may result in flashing lights
  const int STEP_SIZE = 10;
  
  for (unsigned int i = 0; i <= MAX_VALUE; i += STEP_SIZE) {
    // To speed up i2c communication, we only send one update signal
    // each round.
    
    // On our port expander we have 8 IO pins. We can represent all
    // a single byte. We start to build this by initializing it to 0
    uint8_t val = 0;
    // we use the unsigned integer 8 bit type as we shall hold data
    // for 8 port (1 bit per port) and representing it as a signed
    // value does not make sense.
    
    // The port numbers and the binary values it holds is now:
    // port num: 7 6 5 4 3 2 1 0
    // value:    0 0 0 0 0 0 0 0
    
    // we can now loop through all readings to update the byte
    // representation of our outputs
    for (int s = 0; s < 8; ++s) {
      
      // Binary manipulation:
      val |= (readings[s] < i) << s;         // this line can be translated to:
      // uint8_t current = readings[s] < i;  // Comparison will in c/c++ return 1 for true and 0 for false.
      // current = current << s;             // left shift the compared value s times. As s is our sensor number, this means moving the compared value to its correct position in the byte (move it s steps to the left).
      // val = val | current;                // Binary or the two values together.
      
    }
    // After this operation, the uint8_t now holds
    // port num: 7 6 5 4 3 2 1 0
    // value:    x x x x x x x x
    // where 'x' represents the result of the comparison for that particular sensor
    
    // To send the entire data in one operation, we call:
    portExp.writeGPIO(val);
  }
  
  // Make sure all lights are turned off while we do other things
  portExp.writeGPIO(0);
}


void setup() {
  // Console serial communication
  Serial.begin(9600);
  Serial.println("Hello. Initializing zumo and port expander");
  
  // Initialize sensor readings to 0
  for (int i = 0; i < 8; ++i) {
    readings[i] = 0;
  }
  
  // Start communication with port expander
  portExpander_beginAnalog(addr, mcp);

  // Initialize sensor array
  sensors.init();
  
  Serial.println("Done. Starting the light");
}



void loop() {
  // Read all sensor values
  sensors.read(readings);
  
  // From the documentation for the Zumo library we know that sensor values read are in range 0 - 2000
  // We wish to bound these values in range 0 - 100. Only 6 values are read.
  for (int i = 0; i < 6; ++i) {
    readings[i] /= 20;
  }
  
  // Do one cycle of analog write
  portExpander_analogWrite(readings, mcp);
}
