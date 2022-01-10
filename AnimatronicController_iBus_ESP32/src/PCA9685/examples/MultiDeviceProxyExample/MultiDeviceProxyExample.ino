// PCA9685-Arduino Multi-Device Proxy Example
// In this example, we use a special instance to control other modules attached to it via
// the ALL_CALL register.

#include <Wire.h>
#include "PCA9685.h"

PCA9685 pwmController1;                 // Library using default Wire and default linear phase balancing scheme
PCA9685 pwmController2;                 // Library using default Wire and default linear phase balancing scheme

PCA9685 pwmControllerAll;               // Not a real device, will act as a proxy to pwmController1 and pwmController2

void setup() {
    Serial.begin(115200);

    Wire.begin();                       // Wire must be started first
    Wire.setClock(400000);              // Supported baud rates are 100kHz, 400kHz, and 1000kHz

    pwmController1.resetDevices();      // Software resets all PCA9685 devices on Wire line (including pwmController2 in this case)

    pwmController1.init(B000000);       // Address pins A5-A0 set to B000000
    pwmController2.init(B000001);       // Address pins A5-A0 set to B000001

    pwmController1.setChannelOff(0);    // Turn channel 0 off
    pwmController2.setChannelOff(0);    // On both

    pwmController1.enableAllCallAddress(); // Default address of 0xE0
    pwmController2.enableAllCallAddress(); // Same default address

    pwmControllerAll.initAsProxyAddresser(); // Same default address of 0x0E as used in enable above

    pwmControllerAll.setChannelPWM(0, 4096); // Enables full on on both pwmController1 and pwmController2

    Serial.println(pwmController1.getChannelPWM(0)); // Should output 4096
    Serial.println(pwmController2.getChannelPWM(0)); // Should also output 4096

    // Note: Various parts of functionality of the proxy class instance are actually
    // disabled - typically anything that involves a read command being issued.
}

void loop() {
}
