/**
 * Realtime Arduino Animatronic Controller (10 channel, iBus version)
 * 
 * This Arduino script reads serial data from the iBus interface of a RC receiver such as the Flysky FS-iA6B,
 * It then applies various logic to these inputs based on a chosen animatronic model (tentacle, eye, etc.) and assigns corresponding
 * positions of an array of servo motors via a PCA9685 PWM controller.
 */

// INCLUDES
// AltSoftSerial is a better replacement to default Arduino SoftwareSerial library
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
#include "src/AltSoftSerial/AltSoftSerial.h"
#include "src/FlySkyIBus/FlySkyIBus.h"

// CONSTANTS
// How many channels to read from the iBus packet 
const int numInputChannels = 10;

// GLOBALS
// AltSoftSerial received on Pin 8
AltSoftSerial ibusSerial;

void setup() {
  // Software serial connection to the FS-IA6B receiver 
  ibusSerial.begin(115200);
  IBus.begin(ibusSerial);
  // Hardware USB serial connection to the Arduino IDE monitor
  Serial.begin(115200);
  Serial.println("Ready!");
}

void loop() {
 IBus.loop();
 Serial.println(IBus.readChannel(0), HEX);
}
