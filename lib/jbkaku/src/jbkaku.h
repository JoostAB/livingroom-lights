#ifndef JBKAKU_H
#define JBKAKU_H

// Remote control
#define KAKU_ADDRESS_REMOTE 27245058

// Wall switch
#define KAKU_ADDRESS_WALL 25069062

#define WALL_UNIT 0

// https://bitbucket.org/fuzzillogic/433mhzforarduino
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h>

#define TRANSMITPIN D1

#define KAKU_ADDRESS KAKU_ADDRESS_WALL

// Create a transmitter on address KAKU_ADDRESS, using digital pin TRANSMITPIN to transmit, 
// with a period duration of 260ms (default), repeating the transmitted
// code 2^3=8 times.
NewRemoteTransmitter transmitter(KAKU_ADDRESS, TRANSMITPIN, 264, 4);

void setLightsOn() {
    #ifdef DEBUGLOG
    Serial.println("Switching all on...");
    #endif
    
    transmitter.sendUnit(WALL_UNIT, true);
}

void setLightsOff() {
    #ifdef DEBUGLOG
    Serial.println("Switching all off..");
    #endif
    
    transmitter.sendUnit(WALL_UNIT, false);
}

#endif