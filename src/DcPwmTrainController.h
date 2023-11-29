
/**
 * Copyright (c) 2017 Ian Morgan
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License
 */

#pragma once

#include <Arduino.h>
#include <AVR_PWM.h>
#include "TrainController.h"

#define PWM_FREQUENCY  62500     // PWM frequency (in Hz)
#define SPEED_STEP     5

namespace VLCB {

  class DcPwmTrainController: public TrainController {

  private:
    // Speeds here are duty cycle: 0-100, as scaled by speedSteps
    uint8_t  currentLocoSpeed = 0;
    uint8_t  currentLocoDirection = SF_UNHANDLED;
    uint8_t  targetLocoSpeed = 0;
    uint8_t  targetLocoDirection = SF_UNHANDLED;
    uint8_t  speedSteps = 128;
    int      pinA;
    int      pinB;
    int      pinPWM;
    AVR_PWM* pwm;

    void setControllerTargets(int newLocoDirection, int newLocoSpeed);

    // -------------------------------------------------

  public:
    bool eStopped = false;

    DcPwmTrainController(int setPinA,
                         int setPinB,
                         int setPinPWM) {
      pinA = setPinA;
      pinB = setPinB;
      pinPWM = setPinPWM;
      pinMode(pinPWM, OUTPUT);
      pwm = new AVR_PWM(pinPWM, PWM_FREQUENCY, 0);
      pinMode(pinA, OUTPUT);
      digitalWrite(pinA, LOW);
      pinMode(pinB, OUTPUT);
      digitalWrite(pinB, LOW);
    }

    void matchToTargets();
    void emergencyStop();
    void emergencyStopOff();
    bool isEmergencyStop();
    void setSpeedAndDirection(int newLocoDirection, int newLocoSpeed);
    void setSpeed(int newLocoSpeed);
    void setSpeedSteps(int steps);
    uint8_t getSpeed();
    uint8_t getTargetSpeed();
    uint8_t getDirection();
    uint8_t getTargetDirection();
  };

} // VLCB
