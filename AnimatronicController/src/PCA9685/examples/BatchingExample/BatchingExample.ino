// PCA9685-Arduino Batching Example
// In this example, we randomly select PWM frequencies on all 12 outputs and allow them
// to drive for 5 seconds before changing them.

#include <Wire.h>
#include "PCA9685.h"

PCA9685 pwmController;                  // Library using default Wire and default linear phase balancing scheme

void setup() {
    Serial.begin(115200);

    Wire.begin();                       // Wire must be started first
    Wire.setClock(400000);              // Supported baud rates are 100kHz, 400kHz, and 1000kHz

    pwmController.resetDevices();       // Software resets all PCA9685 devices on Wire line

    pwmController.init(B010101);        // Address pins A5-A0 set to B010101
    pwmController.setPWMFrequency(500); // Default is 200Hz, supports 24Hz to 1526Hz

    randomSeed(analogRead(0));          // Use white noise for our randomness
}

void loop() {
    word pwms[12];
    pwms[0] = random(0, 4096);
    pwms[1] = random(0, 4096);
    pwms[2] = random(0, 4096);
    pwms[3] = random(0, 4096);
    pwms[4] = random(0, 4096);
    pwms[5] = random(0, 4096);
    pwms[6] = random(0, 4096);
    pwms[7] = random(0, 4096);
    pwms[8] = random(0, 4096);
    pwms[9] = random(0, 4096);
    pwms[10] = random(0, 4096);
    pwms[11] = random(0, 4096);
    pwmController.setChannelsPWM(0, 12, pwms);
    delay(5000);

    // Note: Only 7 channels can be written in one i2c transaction due to a
    // BUFFER_LENGTH limit of 32, so 12 channels will take two i2c transactions.
}
