#include "MockTrainController.h"

void MockTrainController::matchToTargets() {
}

void MockTrainController::emergencyStop() {
}

void MockTrainController::emergencyStopOff() {
}

bool MockTrainController::isEmergencyStop() {
  return false;
}

void MockTrainController::setSpeedAndDirection(int newLocoDirection, int newLocoSpeed) {
}

void MockTrainController::setSpeed(int newLocoSpeed) {
}

void MockTrainController::setSpeedSteps(int steps) {
}

uint8_t MockTrainController::getSpeed() {
  return 0;
}

uint8_t MockTrainController::getTargetSpeed() {
  return 0;
}

uint8_t MockTrainController::getDirection() {
  return SF_FORWARDS;
}

uint8_t MockTrainController::getTargetDirection() {
  return SF_FORWARDS;
}
