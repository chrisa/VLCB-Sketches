// 3rd party libraries
#include <Streaming.h>

// VLCB library header files
#include <Controller.h>                   // Controller class
#include <CAN2515.h>               // CAN controller
#include <Switch.h>             // pushbutton switch
#include <LED.h>                // VLCB LEDs
#include <Configuration.h>             // module configuration
#include <Parameters.h>             // VLCB parameters
#include <vlcbdefs.hpp>               // VLCB constants
#include <LEDUserInterface.h>
#include "MinimumNodeService.h"
#include "CanService.h"
#include "NodeVariableService.h"
#include "SerialUserInterface.h"
#include "DcPwmTrainController.h"
#include "EventConsumerService.h"
#include "EventTeachingService.h"

// constants
const byte VER_MAJ = 1;             // code major version
const char VER_MIN = 'a';           // code minor version
const byte VER_BETA = 0;            // code beta sub-version
const byte MODULE_ID = 100;          // VLCB module type

const byte LED_GRN = 4;             // VLCB green Unitialised LED pin
const byte LED_YLW = 7;             // VLCB yellow Normal LED pin
const byte SWITCH0 = 8;             // VLCB push button switch pin

// traincontroller pins
const byte TC_PIN_A = 6;
const byte TC_PIN_B = 5;
const byte TC_PIN_PWM = 3;

// Controller objects
VLCB::Configuration modconfig;               // configuration object
VLCB::CAN2515 can2515;                  // CAN transport object
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
VLCB::SerialUserInterface serialUserInterface(&can2515);
VLCB::MinimumNodeService mnService;
VLCB::CanService canService(&can2515);
VLCB::NodeVariableService nvService;
VLCB::DcPwmTrainController trainController(TC_PIN_A, TC_PIN_B, TC_PIN_PWM);
VLCB::EventConsumerService ecService;
VLCB::EventTeachingService etService;

// Controller object
VLCB::Controller controller(&modconfig,
                            { &serialUserInterface, &ledUserInterface, &mnService, &canService, &nvService, &ecService, &etService });


byte canError = 0;
int laps = 0;
long lastBreak = 0;
long lastTrainController = 0;

// module name, must be 7 characters, space padded.
unsigned char mname[7] = { 'D', 'C', 'C', 'M', 'D', ' ', ' ' };

// forward function declarations
void printConfig();
void eventhandler(byte, const VLCB::VlcbMessage *);

//
/// setup VLCB - runs once at power on from setup()
//
void setupVLCB()
{
  // set config layout parameters
  modconfig.EE_NVS_START = 10;
  modconfig.EE_NUM_NVS = 10;
  modconfig.EE_EVENTS_START = 20;
  modconfig.EE_MAX_EVENTS = 32;
  modconfig.EE_PRODUCED_EVENTS = 1;
  modconfig.EE_NUM_EVS = 1;


  // initialise and load configuration
  controller.begin();

  const char * modeString;
  switch (modconfig.currentMode)
  {
    case MODE_NORMAL: modeString = "Normal"; break;
    case MODE_SETUP: modeString = "Setup"; break;
    case MODE_UNINITIALISED: modeString = "Uninitialised"; break;
    default: modeString = "Unknown"; break;
  }
  Serial << F("> mode = (") << _HEX(modconfig.currentMode) << ") " << modeString;
  Serial << F(", CANID = ") << modconfig.CANID;
  Serial << F(", NN = ") << modconfig.nodeNum << endl;

  // show code version and copyright notice
  printConfig();

  // set module parameters
  VLCB::Parameters params(modconfig);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);

  // assign to Controller
  controller.setParams(params.getParams());
  controller.setName(mname);

  // module reset - if switch is depressed at startup and module is in Uninitialised mode
//  if (userInterface.isButtonPressed() && modconfig.currentMode == MODE_UNINITIALISED)
//  {
//    Serial << F("> switch was pressed at startup in Uninitialised mode") << endl;
//    modconfig.resetModule(&userInterface);
//  }

  // opportunity to set default NVs after module reset
  if (modconfig.isResetFlagSet())
  {
    Serial << F("> module has been reset") << endl;
    modconfig.clearResetFlag();
  }

  // register our VLCB event handler, to receive event messages of learned events
  ecService.setEventHandler(eventhandler);

  // set Controller LEDs to indicate mode
  controller.indicateMode(modconfig.currentMode);

  // configure and start CAN bus and VLCB message processing
  can2515.setNumBuffers(2, 1);      // more buffers = more memory used, fewer = less
  can2515.setOscFreq(16000000UL);    // select the crystal frequency of the CAN module
  can2515.setPins(10, 2);           // select pins for CAN bus CE and interrupt connections
  if (!can2515.begin())
  {
    Serial << F("> error starting VLCB") << endl;
  }
}

//
/// setup - runs once at power on
//
void setup()
{
  Serial.begin (115200);
  Serial << endl << endl << F("> ** VLCB DC Command Station v1 ** ") << __FILE__ << endl;

  setupVLCB();

  trainController.setSpeedSteps(128);

  // end of setup
  Serial << F("> ready") << endl << endl;
}

//
/// loop - runs forever
//
void loop()
{
  //
  /// do VLCB message, switch and LED processing
  //
  controller.process();

  // traincontroller timer, every 100ms
  long now = millis();
  if (now - lastTrainController > 100) {
    trainController.matchToTargets();
    lastTrainController = now;
  }

  //
  /// check CAN message buffers
  //
  if (can2515.canp->receiveBufferPeakCount() > can2515.canp->receiveBufferSize())
  {
    Serial << F("> receive buffer overflow") << endl;
  }

  if (can2515.canp->transmitBufferPeakCount(0) > can2515.canp->transmitBufferSize(0))
  {
    Serial << F("> transmit buffer overflow") << endl;
  }

  //
  /// check CAN bus state
  //
  byte s = can2515.canp->errorFlagRegister();
  if (s != canError)
  {
    canError = s;
    Serial << F("> error flag register is non-zero: 0x") << _HEX(canError) << endl;
  }

  // bottom of loop()
}

//
/// print code version config details and copyright notice
//
void printConfig()
{
  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Duncan Greenwood (MERG M5767) 2019") << endl;
}

//
/// user-defined event processing function
/// called from the VLCB library when a learned event is received
/// it receives the event table index and the CAN frame
//
void eventhandler(byte index, const VLCB::VlcbMessage *msg)
{
  Serial << F("> event handler: index = ") << index << F(", opcode = 0x") << _HEX(msg->data[0]) << endl;

  bool ison = (msg->data[0] & 0x01) == 0;

  switch (index) {

  case 0:
    // beam-break

    if (trainController.getSpeed() == 0) {
      // Stopped -- ignore
      Serial << F("beam break while stopped") << endl;
    }
    else {
      // Running, if "on", decrement laps
      if (ison) {
        long now = millis();
        if (now - lastBreak > 5000) {
          laps--;
          Serial << F("lap completed, to go: ") << laps << endl;
        }
        else {
          Serial << F("last break within ") << (now - lastBreak) << F(", not a lap") << endl;
        }
        lastBreak = now;
      }
      // If out of laps, stop.
      if (laps == 0) {
        Serial << F("stopping train") << endl;
        trainController.setSpeed(0);
      }
    }

    break;

  case 1:
    // button push

    if (ison == 1) {
      if (trainController.getSpeed() == 0) {
        // Stopped -- trigger start.
        Serial << F("starting train") << endl;
        trainController.setSpeedAndDirection(SF_FORWARDS, 96);
        laps = 4;
      }
      else {
        Serial << F("button push while running") << endl;
      }
    }

    break;

  default:
    Serial << F("unexpected event index: ") << index << endl;
  }

}
