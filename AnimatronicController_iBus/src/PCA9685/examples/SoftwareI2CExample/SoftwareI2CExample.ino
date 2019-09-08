// PCA9685-Arduino Software I2C Example
// In this example, we utilize the software I2C functionality for chips that do not have
// a hardware I2C bus. We must uncomment the PCA9685_ENABLE_SOFTWARE_I2C define in the
// libraries main header file for software I2C mode to be enabled.

// Uncomment this define to enable use of the software i2c library (min 4MHz+ processor required).
#define PCA9685_ENABLE_SOFTWARE_I2C     1   // http://playground.arduino.cc/Main/SoftwareI2CLibrary

#include "PCA9685.h"

#define SCL_PIN 2                       // Setup defines are written before library include
#define SCL_PORT PORTD 
#define SDA_PIN 0 
#define SDA_PORT PORTC 

#if F_CPU >= 16000000
#define I2C_FASTMODE 1                  // Running a 16MHz processor allows us to use I2C fast mode
#endif

#include "SoftI2CMaster.h"              // Include must come after setup defines

PCA9685 pwmController;                  // Library using default linear phase balancing scheme

void setup() {
    Serial.begin(115200);

    i2c_init();                         // Software I2C must be started first

    pwmController.resetDevices();       // Software resets all PCA9685 devices on software I2C line

    pwmController.init(B000000);        // Address pins A5-A0 set to B000000

    pwmController.setChannelPWM(0, 2048); // Should see a 50% duty cycle along the 5ms phase width
}

void loop() {
}
