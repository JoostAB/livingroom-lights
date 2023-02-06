#ifndef JBKAKU_H
#define JBKAKU_H

// Remote control
#define KAKU_ADDRESS_REMOTE 0x19FBA02 // 27245058
#define KAKU_REMOTE_UNIT0 0
#define KAKU_REMOTE_UNIT1 1
#define KAKU_REMOTE_UNIT2 2

// Wall switch
#define KAKU_ADDRESS_WALL 0x17E8606 // 25069062

#define WALL_UNIT 0

#define TXPIN D1
#define RXPIN D2

#define KAKU_ADDRESS KAKU_ADDRESS_WALL

// https://bitbucket.org/fuzzillogic/433mhzforarduino
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h>

typedef unsigned long t_kakuaddress;

// class JBKakuTx {
//   public:
//     JBKakuTx(),
//     JBKakuTx(byte pin, t_kakuaddress sender);

//     void
//       sendOn(t_kakuaddress target),
//       sendOff(t_kakuaddress target);

//   // private:
//   //   NewRemoteTransmitter _transmitter;
// };

// class JBKakuRx {
//   public:
//     JBKakuRx(),
//     JBKakuRx(byte pin);
// };

typedef std::function<void(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType)> CodeReceivedCB;

CodeReceivedCB _codeReceivedCB;

// Create a transmitter on address KAKU_ADDRESS, using digital pin TRANSMITPIN to transmit, 
// with a period duration of 260ms (default), repeating the transmitted
// code 2^3=8 times.
NewRemoteTransmitter transmitter(KAKU_ADDRESS, TXPIN, 264, 4);

void codeReceived(unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
  if ((address == KAKU_ADDRESS_REMOTE) || (address == KAKU_ADDRESS_WALL)) {
    _codeReceivedCB(address, groupBit, unit, switchType);
  }
}

void startReceiver(CodeReceivedCB codeReceivedCB) {
  _codeReceivedCB = codeReceivedCB;
  NewRemoteReceiver::init(RXPIN,2,codeReceived);
}


void setLightsOn() {
    PRINTLNS("Switching all on...");
    transmitter.sendUnit(WALL_UNIT, true);
}

void setLightsOff() {
    PRINTLNS("Switching all off..");
    transmitter.sendUnit(WALL_UNIT, false);
}


#endif