#include <Streaming.h>
#include <Controller.h>
#include "DcPwmTrainController.h"

namespace VLCB {

// -------------------------------------------------

void DcPwmTrainController::setControllerTargets(int newLocoDirection, int newLocoSpeed) {
        DEBUG_SERIAL << F("setControllerTargets: ") << (newLocoDirection ? F(" forwards ") : F(" reverse "));
        DEBUG_SERIAL << F(" speed ") << newLocoSpeed << F(" speedSteps ") << speedSteps;

        uint8_t speedStepAdjustedSpeed = (uint8_t) newLocoSpeed * 128 / (float) speedSteps;
        uint8_t dutyCycleAdjustedSpeed = (uint8_t) speedStepAdjustedSpeed * 100 / 127;

        DEBUG_SERIAL << F(" speedStepAdjustedSpeed ") << speedStepAdjustedSpeed;
        DEBUG_SERIAL << F(" dutyCycleAdjustedSpeed ") << dutyCycleAdjustedSpeed << endl;

        if (dutyCycleAdjustedSpeed > 100)
                dutyCycleAdjustedSpeed = 100;

        targetLocoSpeed = dutyCycleAdjustedSpeed;
        targetLocoDirection = newLocoDirection;
        eStopped = false;
}

// -------------------------------------------------

void DcPwmTrainController::matchToTargets() {
        // only do anything if speed or direction does not match
        if (eStopped || ((targetLocoSpeed == currentLocoSpeed) && (targetLocoDirection == currentLocoDirection))) {
                return;
        }

        if (targetLocoDirection != currentLocoDirection) {
                // need to decellerate to zero first
                if (currentLocoSpeed < SPEED_STEP) {
                        currentLocoSpeed = 0;
                        currentLocoDirection = targetLocoDirection;
                        pwm->setPWM(pinPWM, PWM_FREQUENCY, 0.0f);
                        if (currentLocoDirection == SF_REVERSE) {
                                digitalWrite(pinA, LOW);
                                digitalWrite(pinB, HIGH);
                        }
                        else {
                                digitalWrite(pinB, LOW);
                                digitalWrite(pinA, HIGH);
                        }
                }
                else {
                        currentLocoSpeed = currentLocoSpeed - SPEED_STEP;
                }
        }
        else {
                // we are going the right direction
                if (currentLocoSpeed > targetLocoSpeed) {
                        // need to decellerate
                        if (targetLocoSpeed + SPEED_STEP > currentLocoSpeed) {
                                currentLocoSpeed = targetLocoSpeed;
                        }
                        else {
                                currentLocoSpeed = currentLocoSpeed - SPEED_STEP;
                        }
                }
                else {
                        // need to accellerate
                        if (currentLocoSpeed + SPEED_STEP > targetLocoSpeed) {
                                currentLocoSpeed = targetLocoSpeed;
                        }
                        else {
                                currentLocoSpeed = currentLocoSpeed + SPEED_STEP;
                        }
                }
        }
        DEBUG_SERIAL<< F("Adusting speed to ") << currentLocoSpeed << endl;
        pwm->setPWM(pinPWM, PWM_FREQUENCY, currentLocoSpeed / 10.0f);
}

// -------------------------------------------------

void DcPwmTrainController::emergencyStop() {
        eStopped = true;
        pwm->setPWM(pinPWM, PWM_FREQUENCY, 0.0f);
        targetLocoSpeed = 0;
        currentLocoSpeed = 0;
        DEBUG_SERIAL << "estop!" << endl;
}

void DcPwmTrainController::emergencyStopOff() {
        eStopped = false;
}

bool DcPwmTrainController::isEmergencyStop() {
        return eStopped;
}

void DcPwmTrainController::setSpeedAndDirection(int newLocoDirection, int newLocoSpeed) {
        setControllerTargets(newLocoDirection, newLocoSpeed);
}

void DcPwmTrainController::setSpeed(int newLocoSpeed) {
        setControllerTargets(currentLocoDirection, newLocoSpeed);
}

void DcPwmTrainController::setSpeedSteps(int steps) {
        speedSteps = steps;
}

uint8_t DcPwmTrainController::getSpeed() {
        return currentLocoSpeed * 127 / 100;
}

uint8_t DcPwmTrainController::getTargetSpeed() {
        return targetLocoSpeed * 127 / 100;
}

uint8_t DcPwmTrainController::getDirection() {
        return currentLocoDirection;
}

uint8_t DcPwmTrainController::getTargetDirection() {
        return targetLocoDirection;
}

} // VLCB
