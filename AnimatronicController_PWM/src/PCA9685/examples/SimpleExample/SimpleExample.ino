// PCA9685-Arduino Simple Example

#include <Wire.h>
#include "PCA9685.h"

PCA9685 pwmController;                  // Library using default Wire and default linear phase balancing scheme

void setup() {
    Serial.begin(115200);

    Wire.begin();                       // Wire must be started first
    Wire.setClock(400000);              // Supported baud rates are 100kHz, 400kHz, and 1000kHz

    pwmController.resetDevices();       // Software resets all PCA9685 devices on Wire line

    pwmController.init(B000000);        // Address pins A5-A0 set to B000000
    pwmController.setPWMFrequency(100); // Default is 200Hz, supports 24Hz to 1526Hz

    pwmController.setChannelPWM(0, 128 << 4); // Set PWM to 128/255, but in 4096 land

    Serial.println(pwmController.getChannelPWM(0)); // Should output 2048, which is 128 << 4
}

void loop() {
}
