/**
 * Realtime Arduino Animatronic Controller (10 channel, iBus version)
 * 
 * This Arduino script reads serial data from the iBus interface of a RC receiver such as the Flysky FS-iA6B,
 * It then applies various logic to these inputs based on a chosen animatronic model (tentacle, eye, etc.) and assigns corresponding
 * positions of an array of servo motors via a PCA9685 PWM controller.
 */

// DEFINES
// Hardware UART port to use for iBus interface
#define iBusSerial Serial2

// INCLUDES
// Wire library used for I2C communication
#include <Wire.h>
// PCA9685 16-channel PWM controller
// See: https://github.com/NachtRaveVL/PCA9685-Arduino
#include "src/PCA9685/PCA9685.h"
// FlySky iBus interface adapted from https://gitlab.com/timwilkinson/FlySkyIBus
#include "src/FlySkyIBus/FlySkyIBus.h"

// CONSTANTS
// How many channels to read from the iBus packet (max 14)
const int numInputChannels = 10;
// This is the number of output channels (i.e. servo motors) to be controlled by the script
// Note that the logic applied by the script means that there is not a 1:1 mapping between inputs and outputs
// so this number can be greater or less than the number of inputs 
const byte numOutputChannels = 4;
// Define some common animatronic behaviour types
enum Behaviour {
  SimpleTentacle, // X/Y tentacle movement only e.g. https://www.thingiverse.com/thing:6986
  WigglyTentacle, // X/Y tentacle with random wigggles added.
  DualTentacles, // 2x independent X/Y movement
  SimpleEye, // X movement of eye, Y movement of eyelids
  Eye,
};

// GLOBALS

// Instantiate an IBus object to process IBus data
FlySkyIBus iBus;
// Initialise the PWM controller which will send output to the servos
PCA9685 pwmController;
PCA9685_ServoEvaluator pwmServo;
// Define the behavior which we'd like the model to follow
Behaviour behaviour = Behaviour::SimpleTentacle;
// Array of input channel values read from the iBus packet received from FS-iA6B
int channelInput[numInputChannels];
// Array of output channel values to send to PCA9685
int channelOutput[numOutputChannels];

void setup() {
  // Software serial connection to the FS-IA6B receiver 
  iBusSerial.begin(115200);
  // Attach the IBus object to the ibusSerial interface
  iBus.begin(iBusSerial);

  // Initialise I2C interface used for the PCA9685 PWM controller
  Wire.begin();
  // Supported baud rates are 100kHz, 400kHz, and 1000kHz
  Wire.setClock(400000);
  // Initialise PCA9685
  pwmController.resetDevices();
  pwmController.init();
  // 50Hz provides 20ms standard servo phase length
  pwmController.setPWMFrequency(50);
  
  // Hardware USB serial connection to the Arduino IDE monitor
  Serial.begin(115200);
  Serial.println("Ready!");
}

void loop() {
  // iBus data is sent is discrete packets. We'll call IBus.loop() on every
  // frame to check whether a new packet has been received. If so, it will return true.
  if(iBus.loop()) {

    for(int i=0; i<numInputChannels; i++) {
      channelInput[i] = iBus.readChannel(i);
      Serial.print(channelInput[i]);
      Serial.print(",");
    }
    Serial.println("");

    // Now perform whatever logic is necessary to convert from the array of input channels
    // into the array of output values
    ApplyLogic();
  
    // Now, send the output values to the PWM servo controller
    for(int i=0; i<numOutputChannels; i++) {
      pwmController.setChannelPWM(i, channelOutput[i]);
    }
  }
}

// This function processes inputs and calculates the appropriate outputs
// based on the selected model
void ApplyLogic(){
  // Behaviour depends on chosen model
  switch(behaviour) {
    
    case Behaviour::SimpleTentacle: {
      // Calculate movement in X axis
      int X = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X = map(channelInput[0], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 0
      channelOutput[0] = pwmServo.pwmForAngle(X);

      // Calculate movement in the Y axis
      int Y = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y = map(channelInput[1], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 1
      channelOutput[1] = pwmServo.pwmForAngle(Y);
      }
      break;

    case Behaviour::WigglyTentacle: {
      // Calculate movement in X axis
      int X = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X = map(channelInput[0], 1000, 2000, -90, 90);
      }
      // Calculate additional periodic wiggly offset
      X += 6 * sin(millis() * 0.005);
      // Calculate PWM corresponding to desired angle and assign to output channel 0
      channelOutput[0] = pwmServo.pwmForAngle(X);

      // Calculate movement in Y axis
      int Y = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y = map(channelInput[1], 1000, 2000, -90, 90);
      }
      // Calculate additional periodic wiggly offset
      Y += 5 * cos(millis() * 0.0051);

      // Calculate PWM corresponding to desired angle and assign to output channel 1
      channelOutput[1] = pwmServo.pwmForAngle(Y);
      }
      break;


    case Behaviour::DualTentacles: {
      int X1 = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -90 to +90
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X1 = map(channelInput[0], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 0
      channelOutput[0] = pwmServo.pwmForAngle(X1);

      int Y1 = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y1 = map(channelInput[1], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 1
      channelOutput[1] = pwmServo.pwmForAngle(Y1);

      int X2 = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[2] < 2000) { 
        X2 = map(channelInput[0], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 2
      channelOutput[2] = pwmServo.pwmForAngle(X2);

      int Y2 = 0;
      if(channelInput[1] > 995 && channelInput[3] < 2000) { 
        Y2 = map(channelInput[1], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 3
      channelOutput[3 ] = pwmServo.pwmForAngle(Y2);
      }
      break;

    case Behaviour::SimpleEye: {
      // EYE LEFT/RIGHT
      int eyeX = 0;
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        // The ON period of almost all RC pulses range from 1000us to 2000us.
        // We'll remap this to an angle from -90 to +90
        eyeX = map(channelInput[0], 1000, 2000, -90, 90);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 0
      channelOutput[0] = pwmServo.pwmForAngle(eyeX);

      // EYELID UP/DOWN
      int eyelidY = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        eyelidY = map(channelInput[1], 1000, 2000, -45, 90);
      }
      // CHANCE OF RANDOM BLINK
      static unsigned long nextBlinkTime;
      if(millis() > nextBlinkTime && millis() < nextBlinkTime + 200) {
        eyelidY = -45;
      }
      else if(millis() > nextBlinkTime + 200) {
        nextBlinkTime += random(5000, 8000);
      }
      // Calculate PWM corresponding to desired angle and assign to output channel 1
      channelOutput[1] = pwmServo.pwmForAngle(eyelidY);
      }
      break;

    case Behaviour::Eye: {
     // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      int eyeX = map(channelInput[0], 1000, 2000, -30, 30);
      channelOutput[0] = pwmServo.pwmForAngle(eyeX);
      int eyeY = map(channelInput[1], 1000, 2000, -25, 25);
      channelOutput[1] = pwmServo.pwmForAngle(eyeY);
    
      int eyesWideOpen = map(channelInput[2], 1000, 2000, -10, 10);
      int topEyelid = eyesWideOpen + eyeY / 2;
      channelOutput[2] = pwmServo.pwmForAngle(topEyelid);
      int bottomEyelid = eyesWideOpen - eyeY / 2;
      channelOutput[3] = pwmServo.pwmForAngle(bottomEyelid);
      }
      break;
    
    default: 
      break;
  }
}
