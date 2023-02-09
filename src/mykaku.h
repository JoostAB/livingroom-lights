/**
 * @file mykaku.h
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-09
 */
# pragma once
#ifndef __MY_KAKU_H__
#define __MY_KAKU_H__

#include <Arduino.h>
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h>
#include <jbdebug.h>


#define TXPIN D2  // Pin for Transmitter
#define RXPIN D1  // Pin for Receiver

#define KAKU_ADDRESS_REMOTE 0x19FBA02 // 27245058
#define KAKU_REMOTE_UNIT0 0
#define KAKU_REMOTE_UNIT1 1
#define KAKU_REMOTE_UNIT2 2

// Wall switch
#define KAKU_ADDRESS_WALL 0x17E8606 // 25069062

#define WALL_UNIT 0

typedef unsigned long t_kakuaddress;

typedef std::function<void(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType)> CodeReceivedCB;

NewRemoteTransmitter transmitter(KAKU_ADDRESS_WALL, TXPIN, 264, 4);

CodeReceivedCB _codeReceivedCB;

void codeReceived(unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
  PRINTLN("Code (address): ", address);
  PRINTLN("Period: ", period);
  PRINTLN("unit: ", unit);
  PRINTLN("groupBit: ", groupBit);
  PRINTLN("switchType: ", switchType);

  IFDEBUG(
    if (dimLevelPresent){
      PRINTLN("dimLevel: ", dimLevel);    
    }
  )

  if (_codeReceivedCB != nullptr) {
    _codeReceivedCB(address, groupBit, unit, switchType);
  } else {
    PRINTLNS("No callback for KAKU CodeReceived");
  }
}

void kaku_start(CodeReceivedCB codeReceivedCB) {
  _codeReceivedCB = codeReceivedCB;
  if (_codeReceivedCB == nullptr) {
    PRINTLNS("No callback for KAKU CodeReceived");
  }
  PRINTLNS("Initializing KaKu receiver");
  NewRemoteReceiver::init(RXPIN,2,codeReceived);
  PRINTLN("Initialized KaKu receiver on pin ",RXPIN);
  
}

void _kaku_send_unit(bool sw) {
  // Disable receiver while sending command
  NewRemoteReceiver::disable();
  transmitter.sendUnit(WALL_UNIT, sw);
  NewRemoteReceiver::enable();
}

void kaku_setLightsOn() {
  PRINTLNS("Switching all on...");
  _kaku_send_unit(true);
}

void kaku_setLightsOff() {
  PRINTLNS("Switching all off..");
  _kaku_send_unit(false);
}

#endif // __MY_KAKU_H__