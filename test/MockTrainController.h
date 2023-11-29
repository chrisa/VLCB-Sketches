//  Copyright (C) Sven Rosvall (sven@rosvall.ie)
//  This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
//  Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
//  The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0
//
//

#pragma once

#include "TrainController.h"

class MockTrainController : public VLCB::TrainController
{
public:
  MockTrainController();
  virtual void matchToTargets();
  virtual void emergencyStop();
  virtual void emergencyStopOff();
  virtual bool isEmergencyStop();
  virtual void setSpeedAndDirection(int newLocoDirection, int newLocoSpeed);
  virtual void setSpeed(int newLocoSpeed);
  virtual void setSpeedSteps(int steps);
  virtual uint8_t getSpeed();
  virtual uint8_t getTargetSpeed();
  virtual uint8_t getDirection();
  virtual uint8_t getTargetDirection();
};
