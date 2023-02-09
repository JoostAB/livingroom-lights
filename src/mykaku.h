#include <Arduino.h>
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h>
#include <jbdebug.h>

#define TXPIN D2
#define RXPIN D1

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
  Serial.print("Code (address): ");
  Serial.println(address);
  Serial.print(" Period: ");
  Serial.println(period);
  Serial.print(" unit: ");
  Serial.println(unit);
  Serial.print(" groupBit: ");
  Serial.println(groupBit);
  Serial.print(" switchType: ");
  Serial.println(switchType);

  if (dimLevelPresent){
    Serial.print(" dimLevel: ");
    Serial.println(dimLevel);    
  }

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
  NewRemoteReceiver::init(5,2,codeReceived);
  PRINTLN("Initialized KaKu receiver on pin ",RXPIN);
  
}

void kaku_setLightsOn() {
    PRINTLNS("Switching all on...");
    NewRemoteReceiver::disable();
    transmitter.sendUnit(WALL_UNIT, true);
    NewRemoteReceiver::enable();
}

void kaku_setLightsOff() {
    PRINTLNS("Switching all off..");
    NewRemoteReceiver::disable();
    transmitter.sendUnit(WALL_UNIT, false);
    NewRemoteReceiver::enable();
}