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
// FlySky iBus interface adapted from https://gitlab.com/timwilkinson/FlySkyIBus
#include "src/FlySkyIBus/FlySkyIBus.h"

// CONSTANTS
// How many channels to read from the iBus packet (max 14)
const int numInputChannels = 10;

// GLOBALS
// AltSoftSerial receives serial data on Pin 8
AltSoftSerial iBusSerial;
// Instantiate an IBus object to process IBus data
FlySkyIBus iBus;

void setup() {
  // Software serial connection to the FS-IA6B receiver 
  iBusSerial.begin(115200);
  // Attach the IBus object to the ibusSerial interface
  iBus.begin(iBusSerial);
  // Hardware USB serial connection to the Arduino IDE monitor
  Serial.begin(115200);
  Serial.println("Ready!");
}

void loop() {
  // iBus data is sent is discrete packets. We'll call IBus.loop() on every
  // frame to check whether a new packet has been received. If so, it will return true.
  if(iBus.loop()) {
    for(int i=0; i<numInputChannels; i++) {
      Serial.print(iBus.readChannel(i));
      Serial.print(",");
    }
    Serial.println("");
  }
}
