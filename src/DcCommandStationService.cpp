// Copyright (C) Martin Da Costa 2023 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include <Streaming.h>

#include <DcCommandStationService.h>
#include <Controller.h>
#include <TrainController.h>
#include <vlcbdefs.hpp>

namespace VLCB {

  DcCommandStationService::DcCommandStationService(TrainController *tc)
    : tc(tc)
  {
    this->timerMillis = millis();
  }

  void DcCommandStationService::setController(Controller *cntrl)
  {
    this->controller = cntrl;
    this->module_config = cntrl->getModuleConfig();
  }

  void DcCommandStationService::process(UserInterface::RequestedAction requestedAction)
  {
    unsigned long now = millis();
    if (now > (timerMillis + TIMER_MS)) {
      timerMillis = now;
      tc->matchToTargets();
    }
  }

  void DcCommandStationService::sendMessage(unsigned int opc, unsigned int len, ...)
  {
    va_list args;
    va_start(args, len);

    VlcbMessage msg;
    msg.len = len + 1;
    msg.data[0] = opc;
    for (int i = 0 ; i < len ; ++i)
    {
      msg.data[1 + i] = va_arg(args, int);
    }
    va_end(args);
    controller->sendMessage(&msg);
  }

  Processed DcCommandStationService::handleMessage(unsigned int opc, VlcbMessage *msg)
  {
    //DEBUG_SERIAL << ">Handle Message " << endl;
    unsigned int nn = (msg->data[1] << 8) + msg->data[2];
    unsigned int en = (msg->data[3] << 8) + msg->data[4];
    //DEBUG_SERIAL << ">DcCommandStationService handling message op=0x" << _HEX(opc) << " nn=" << nn << " en=" << en << endl;

    switch (opc)
      {
      case OPC_RSTAT:
        controller->sendMessageWithNN(OPC_STAT, 1, 0, 7, 8, 1);
        return PROCESSED;

      case OPC_RESTP:
      case OPC_RTOF:
        tc->emergencyStop();
        controller->sendMessageWithNN(OPC_TOF);
        return PROCESSED;

      case OPC_RTON:
        tc->emergencyStopOff();
        controller->sendMessageWithNN(OPC_TON);
        return PROCESSED;

      case OPC_RLOC:
        this->sendMessage(OPC_PLOC, 4, 0, (nn >> 8) & 0x3f, nn & 0xff, tc->getSpeed() | (tc->getDirection() ? 0x80 : 0));
        return PROCESSED;

      case OPC_KLOC:
        tc->setSpeedAndDirection(SF_UNHANDLED, 0);
        return PROCESSED;

      case OPC_DSPD:
        if (nn < 128) {
          tc->setSpeedAndDirection(SF_REVERSE, nn);
        }
        else {
          tc->setSpeedAndDirection(SF_FORWARDS, nn - 128);
        }
        return PROCESSED;

      case OPC_STMOD:
        switch (nn)
          {
          case 3:
            tc->setSpeedSteps(28);
            break;
          case 1:
            tc->setSpeedSteps(14);
            break;
          case 0:
            tc->setSpeedSteps(128);
            break;
          }
        return PROCESSED;

      case OPC_DKEEP:
        return PROCESSED;

      default:
        // unknown or unhandled OPC
        DEBUG_SERIAL << F("> DCSS opcode 0x") << _HEX(opc) << F(" is not currently implemented")  << endl;
        return NOT_PROCESSED;
      }
  }
}
