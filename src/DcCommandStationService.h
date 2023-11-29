// Copyright (C) Martin Da Costa 2023 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#pragma once

#include "Service.h"
#include <vlcbdefs.hpp>

#include "TrainController.h"

#define TIMER_MS 500

namespace VLCB {

class Configuration;

class DcCommandStationService : public Service {
public:
  DcCommandStationService(TrainController *tc);

  virtual void setController(Controller *cntrl) override;
  virtual void process(UserInterface::RequestedAction requestedAction) override;
  virtual Processed handleMessage(unsigned int opc, VlcbMessage *msg) override;

  virtual byte getServiceID() override
  {
    return SERVICE_ID_CONSUMER;
  }
  virtual byte getServiceVersionID() override
  {
    return 1;
  }

private:
  Controller *controller;
  Configuration *module_config;  // Shortcut to reduce indirection code.
  TrainController *tc;
  unsigned long timerMillis;

  void sendMessage(unsigned int opc, unsigned int len, ...);
};

}  // VLCB
