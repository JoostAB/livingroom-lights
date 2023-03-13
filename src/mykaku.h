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

#include <general.h>
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h>

#ifdef ESP32
  #define TXPIN 1  // Pin for Transmitter
  #define RXPIN 2  // Pin for Receiver
#else
  #define TXPIN D2  // Pin for Transmitter
  #define RXPIN D1  // Pin for Receiver
#endif

#define KAKU_ADDRESS_REMOTE 0x19FBA02 // 27245058
#define KAKU_REMOTE_UNIT0 0
#define KAKU_REMOTE_UNIT1 1
#define KAKU_REMOTE_UNIT2 2

// Wall switch
#define KAKU_ADDRESS_WALL 0x17E8606 // 25069062

#define WALL_UNIT 0

typedef unsigned long t_kakuaddress;

/**
 * @brief Callback signature for function to be called whenever a valid KaKu command is received
 * 
 */
typedef std::function<void(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType)> CodeReceivedCB;

NewRemoteTransmitter transmitter(KAKU_ADDRESS_WALL, TXPIN, 264, 4);

CodeReceivedCB _codeReceivedCB;

/**
 * @brief Called by NewRemoteReceiver when a valid KAKU code is received. Will pass is on to the
 * CodeReceivedCB callback registered in kaku_start
 * 
 * @param period 
 * @param address 
 * @param groupBit 
 * @param unit 
 * @param switchType 
 * @param dimLevelPresent 
 * @param dimLevel 
 */
void _kaku_codeReceived(unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
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

/**
 * @brief Initializes the KaKu classes
 * 
 * @param codeReceivedCB function to be called whenever a KaKu command is received
 */
void kaku_start(CodeReceivedCB codeReceivedCB) {
  _codeReceivedCB = codeReceivedCB;
  if (_codeReceivedCB == nullptr) {
    PRINTLNS("No callback for KAKU CodeReceived");
  }
  PRINTLNS("Initializing KaKu receiver");
  NewRemoteReceiver::init(RXPIN,2,_kaku_codeReceived);
  PRINTLN("Initialized KaKu receiver on pin ",RXPIN);
  
}

/**
 * @brief Send ON or OFF mimmicking the WALL unit
 * Disables the receiver while sending to prevent loops/echo
 * 
 * @param sw true = ON, false = OFF
 */
void _kaku_send_unit(bool sw) {
  // Disable receiver while sending command
  NewRemoteReceiver::disable();
  transmitter.sendUnit(WALL_UNIT, sw);
  NewRemoteReceiver::enable();
}

/**
 * @brief Switch the lights on
 * 
 */
void kaku_setLightsOn() {
  PRINTLNS("Switching all on...");
  _kaku_send_unit(true);
}

/**
 * @brief Switch the lights off
 * 
 */
void kaku_setLightsOff() {
  PRINTLNS("Switching all off..");
  _kaku_send_unit(false);
}

/**
 * @brief Switch the lights on or off
 * 
 * @param sw true = on, false = off
 */
void kaku_setLights(bool sw) {
  if (sw) {
    kaku_setLightsOn();
  } else {
    kaku_setLightsOff();
  }
}

#endif // __MY_KAKU_H__