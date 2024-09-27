// 3rd party libraries
#include <Streaming.h>
#include <PCF8574.h>
#include <Wire.h>

// VLCB library header files
#include <Controller.h>
#include <CAN2515.h>
#include <Switch.h>
#include <Configuration.h>
#include <Parameters.h>
#include <vlcbdefs.hpp>
#include "MinimumNodeService.h"
#include "CanService.h"
#include "LEDUserInterface.h"
#include "NodeVariableService.h"
#include "SerialUserInterface.h"
#include "EventConsumerService.h"
#include "EventProducerService.h"
#include "ConsumeOwnEventsService.h"
#include "EventTeachingService.h"
#include "SerialUserInterface.h"

#include <Arduino.h>

// constants
const byte VER_MAJ = 1;             // code major version
const char VER_MIN = 'a';           // code minor version
const byte VER_BETA = 0;            // code beta sub-version
const byte MODULE_ID = 99;          // VLCB module type

const byte LED_GRN = 4;             // VLCB green Unitialised LED pin
const byte LED_YLW = 7;             // VLCB yellow Normal LED pin
const byte SWITCH0 = 8;             // VLCB push button switch pin

#define PCF8574_BASE 0x20
#define PCF8574_MAX 8

// Controller objects
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
VLCB::Configuration modconfig;               // configuration object
VLCB::CAN2515 can2515;                  // CAN transport object
VLCB::SerialUserInterface serialUserInterface(&can2515);
VLCB::MinimumNodeService mnService;
VLCB::CanService canService(&can2515);
VLCB::NodeVariableService nvService;
VLCB::EventConsumerService ecService;
VLCB::EventProducerService epService;
VLCB::EventTeachingService etService;
VLCB::ConsumeOwnEventsService coeService;

VLCB::Controller controller(&modconfig,
                            { &serialUserInterface, &ledUserInterface, &mnService, &canService, &nvService, &ecService, &epService, &etService, &coeService });

byte canError = 0;
PCF8574 *devices[PCF8574_MAX];

// module name, must be 7 characters, space padded.
unsigned char mname[7] = { 'I', '2', 'C', ' ', ' ', ' ', ' ' };

void eventhandler(byte, const VLCB::VlcbMessage *);

void printConfig() {
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
}

void setupVLCB() {
  modconfig.EE_NVS_START = 10;
  modconfig.EE_NUM_NVS = 192;
  modconfig.EE_EVENTS_START = 202;
  modconfig.EE_MAX_EVENTS = 32;
  modconfig.EE_PRODUCED_EVENTS = 1;
  modconfig.EE_NUM_EVS = 1;

  controller.begin();

  const char * modeString;
  switch (modconfig.currentMode) {
  case MODE_NORMAL: modeString = "Normal"; break;
  case MODE_SETUP: modeString = "Setup"; break;
  case MODE_UNINITIALISED: modeString = "Uninitialised"; break;
  default: modeString = "Unknown"; break;
  }
  Serial << F("> mode = (") << _HEX(modconfig.currentMode) << ") " << modeString;
  Serial << F(", CANID = ") << modconfig.CANID;
  Serial << F(", NN = ") << modconfig.nodeNum << endl;

  // set module parameters
  VLCB::Parameters params(modconfig);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);

  // assign to Controller
  controller.setParams(params.getParams());
  controller.setName(mname);

  // module reset - if switch is depressed at startup and module is in Uninitialised mode
  if (ledUserInterface.isButtonPressed() && modconfig.currentMode == MODE_UNINITIALISED) {
    Serial << F("> switch was pressed at startup in Uninitialised mode") << endl;
    modconfig.resetModule();
  }

  // opportunity to set default NVs after module reset
  if (modconfig.isResetFlagSet()) {
    Serial << F("> module has been reset") << endl;
    modconfig.clearResetFlag();
  }

  // register our VLCB event handler, to receive event messages of learned events
  ecService.setEventHandler(eventhandler);

  // set Controller LEDs to indicate mode
  controller.indicateMode(modconfig.currentMode);

  // configure and start CAN bus and VLCB message processing
  can2515.setNumBuffers(2, 1);      // more buffers = more memory used, fewer = less
  can2515.setOscFreq(8000000UL);    // select the crystal frequency of the CAN module
  can2515.setPins(10, 2);           // select pins for CAN bus CE and interrupt connections
  if (!can2515.begin()) {
    Serial << F("> error starting VLCB") << endl;
  }
}

// NVs layout
// 3 NVs per pin
// 0 - flags (incl type)
// 1 - on delay / pulse duration
// 2 - off delay / flash period
//
// flags:
// 11111111
// |||||||\--- type (0 - INPUT / 1 - OUTPUT)
// ||||||\---- trigger-inverted
// |||||\----- event-inverted
// ||||\------ disable-off
// |||\------- toggle (in) / startup (out)
// ||\-------- action-inverted
// |\--------- disable-sod
// \---------- action-expedited

#define NV_FLAG_TYPE 0x01
#define NV_FLAG_TRIG_INV 0x02
#define NV_FLAG_EVENT_INV 0x04
#define NV_FLAG_DISABLE_OFF 0x08
#define NV_FLAG_TOGGLE 0x0f
#define NV_FLAG_STARTUP 0x10
#define NV_FLAG_ACTION_INV 0x20
#define NV_FLAG_DISABLE_SOD 0x40
#define NV_FLAG_EXPEDITED 0x80

#define PIN_TYPE(flags) ((flags & NV_FLAG_TYPE) ? OUTPUT : INPUT)
#define PIN_NV_FLAGS(device, pin) ((((device * 8) + pin) * 3) + 1)
#define PIN_NV_ON_PULSE(device, pin) (PIN_NV_FLAGS(device, pin) + 1)
#define PIN_NV_OFF_PERIOD(device, pin) (PIN_NV_FLAGS(device, pin) + 2)

void setup() {
  Serial.begin (115200);
  setupVLCB();

  Wire.begin();
  Serial << F("Scanning for PCF8574 Devices") << endl;

  for (byte offset = 0; offset < PCF8574_MAX; offset++) {
    byte address = PCF8574_BASE + offset;
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial << F("I2C device found at address 0x") << _HEX(address) << (" !") << endl;
      devices[offset] = new PCF8574(address);
    }
    else {
      devices[offset] = nullptr;
    }
  }

  for (byte i = 0; i < 8; i++) {
    if (devices[i] != nullptr) {
      for (byte j = 0; j < 8; j++) {
        byte flags = modconfig.readNV(PIN_NV_FLAGS(i, j));
        Serial << F("Device ") << i << F(" Pin ") << j << F(" is ") << (PIN_TYPE(flags) == INPUT ? "input" : "output") << endl;
        devices[i]->pinMode(j, PIN_TYPE(flags), LOW);
      }

      devices[i]->begin();

      for (byte j = 0; j < 8; j++) {
        devices[i]->digitalWrite(j, LOW);
      }
    }
  }

  // end of setup
  Serial << F("> ready") << endl << endl;
}

void loop() {
  controller.process();

  if (can2515.canp->receiveBufferPeakCount() > can2515.canp->receiveBufferSize()) {
    Serial << F("> receive buffer overflow") << endl;
  }

  if (can2515.canp->transmitBufferPeakCount(0) > can2515.canp->transmitBufferSize(0)) {
    Serial << F("> transmit buffer overflow") << endl;
  }

  byte s = can2515.canp->errorFlagRegister();
  if (s != canError) {
    canError = s;
    Serial << F("> error flag register is non-zero: 0x") << _HEX(canError) << endl;
  }
}

void eventhandler(byte index, const VLCB::VlcbMessage *msg) {
  bool ison = (msg->data[0] & 0x01) == 0;
  byte evval = modconfig.getEventEVval(index, 1);

  byte dev = (evval & 0xf0) >> 4;
  byte pin = (evval & 0x0f);

  if (devices[dev] != nullptr && pin < 8) {
      devices[dev]->digitalWrite(pin, ison ? HIGH : LOW);
  }
  else {
    Serial << F("unexpected event configuration, dev = ") << dev << F(" pin = ") << pin << endl;
  }
}
