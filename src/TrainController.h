
/**
 * Copyright (c) 2017 Ian Morgan
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License
 */

#pragma once

#define SF_FORWARDS    0x01      // Train is running forwards
#define SF_REVERSE     0x00      // Train is running in reverse
#define SF_UNHANDLED   0xFF      // Train is not running

#include <Arduino.h>

namespace VLCB {

  class TrainController {

  public:
    virtual void matchToTargets() = 0;
    virtual void emergencyStop() = 0;
    virtual void emergencyStopOff() = 0;
    virtual bool isEmergencyStop() = 0;
    virtual void setSpeedAndDirection(int newLocoDirection, int newLocoSpeed) = 0;
    virtual void setSpeed(int newLocoSpeed) = 0;
    virtual void setSpeedSteps(int steps) = 0;
    virtual uint8_t getSpeed() = 0;
    virtual uint8_t getTargetSpeed() = 0;
    virtual uint8_t getDirection() = 0;
    virtual uint8_t getTargetDirection() = 0;
    // TODO DCC ID setting methods etc.
  };

} // VLCB
