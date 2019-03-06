// Arduino pins are grouped into 3 "ports" that share a pin change interrupt
// Set the PORTS_TO_USE value to specify which group of pins to use to read the RC ahnnel inputs
// PORTS_TO_USE  1 for pins D8-D13
// PORTS_TO_USE 2 for pins A0-A5
// PORTS_TO_USE 3 for pins D0-D7
#define PORTS_TO_USE 3

//INCLUDES
// Wire library used for I2C communication
#include <Wire.h>
// PCA9685 16-channel PWM controller
// https://github.com/NachtRaveVL/PCA9685-Arduino
#include "src/PCA9685/PCA9685.h"
// Helper library for reading PPM signals from RC receiver
// https://github.com/timoxd7/FastRCReader
#include "src/FastRCReader/FastRCReader.h"

// CONSTANTS
// I'm using a FS-IA6 6-channel receiver
const int numInputChannels = 6;
// Each channel will be assigned a unique pin in the range defined by PORTS_TO_USE
uint8_t channelPins[numInputChannels] = {7,6,5,4,3,2};
const byte numOutputChannels = 4;

enum Behaviour {
  SimpleTentacle, // X/Y tentacle movement only e.g. https://www.thingiverse.com/thing:6986
  WigglyTentacle, // X/Y tentacle with random wigggles added.
  DualTentacles, // X/Y x 2
  SimpleEye, // X movement of eye, Y movement of eyelids
  Eye,
};

// GLOBALS
PCA9685 pwmController;
PCA9685_ServoEvaluator pwmServo;
FastRCReader RC;
Behaviour behaviour = Behaviour::DualTentacles;

// Array of current channel input values
int channelInput[numInputChannels];
int channelOutput[numOutputChannels];

// This function processes inputs and clacualted the appropriate outputs.
// It applies different behaviour for, say, a tentacle or a pair of eyes
void ApplyLogic(){

  switch(behaviour) {

    case Behaviour::SimpleTentacle: 
    {
      int X = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X = map(channelInput[0], 1000, 2000, -90, 90);
      }
      //Serial.println(X);
      channelOutput[0] = pwmServo.pwmForAngle(X);

      int Y = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y = map(channelInput[1], 1000, 2000, -90, 90);
      }
      channelOutput[1] = pwmServo.pwmForAngle(Y);
    }
      break;

    case Behaviour::WigglyTentacle: 
    {
      int X = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X = map(channelInput[0], 1000, 2000, -90, 90);
      }
      float theta = millis() * 0.005;
      X += 6 * sin(theta);   
      channelOutput[0] = pwmServo.pwmForAngle(X);

      int Y = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y = map(channelInput[1], 1000, 2000, -90, 90);
      }
      Y += 5 * cos(theta * 1.01);
      channelOutput[1] = pwmServo.pwmForAngle(Y);
    }
      break;


    case Behaviour::DualTentacles: 
    {
      int X1 = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        X1 = map(channelInput[0], 1000, 2000, -90, 90);
      }
      //Serial.println(X);
      channelOutput[0] = pwmServo.pwmForAngle(X1);

      int Y1 = 0;
      if(channelInput[1] > 995 && channelInput[1] < 2000) { 
        Y1 = map(channelInput[1], 1000, 2000, -90, 90);
      }
      channelOutput[1] = pwmServo.pwmForAngle(Y1);

      int X2 = 0;
      // The ON period of almost all RC pulses range from 1000us to 2000us.
      // We'll remap this to an angle from -45 to +45
      if(channelInput[0] > 995 && channelInput[2] < 2000) { 
        X2 = map(channelInput[0], 1000, 2000, -90, 90);
      }
      //Serial.println(X);
      channelOutput[2] = pwmServo.pwmForAngle(X2);

      int Y2 = 0;
      if(channelInput[1] > 995 && channelInput[3] < 2000) { 
        Y2 = map(channelInput[1], 1000, 2000, -90, 90);
      }
      channelOutput[3 ] = pwmServo.pwmForAngle(Y2);
      
    }
    break;


    case Behaviour::SimpleEye: 
    {
      // EYE LEFT/RIGHT
      int eyeX = 0;
      if(channelInput[0] > 995 && channelInput[0] < 2000) { 
        // The ON period of almost all RC pulses range from 1000us to 2000us.
        // We'll remap this to an angle from -45 to +45
        eyeX = map(channelInput[0], 1000, 2000, -90, 90);
      }
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
      channelOutput[1] = pwmServo.pwmForAngle(eyelidY);
    }
    break;

    case Behaviour::Eye: 
    {
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


void setup() {
  // Initialise the serial connection
  Serial.begin(9600);

  RC.begin();
  RC.addChannel(channelPins, numInputChannels);

  // Initialise I2C interface used 
  Wire.begin();
  // Supported baud rates are 100kHz, 400kHz, and 1000kHz
  Wire.setClock(400000);

    pwmController.resetDevices();       // Software resets all PCA9685 devices on Wire line

    pwmController.init();        // Address pins A5-A0 set to B000000
    pwmController.setPWMFrequency(50);  // 50Hz provides 20ms standard servo phase length

//    pwmController.setChannelPWM(0, 128 << 4); // Set PWM to 128/255, but in 4096 land

}

void loop() {
  // Read all the current channel input values into the input array
  for(int i=0; i<numInputChannels; i++) {
    channelInput[i] = RC.getFreq(channelPins[i]);
      Serial.print(channelInput[i]);
      Serial.print(",");
  }
  Serial.println();

  
  // Now perform whatever logic is necessary
  ApplyLogic();
  
  for(int i=0; i<numOutputChannels; i++) {
    pwmController.setChannelPWM(i, channelOutput[i]);
  }
}
