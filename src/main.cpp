#include <Arduino.h>

#define DEBUGLOG 1

const char* cmdOn = "ON";
const char* cmdOff = "OFF";

#include <jbdebug.h>
#include <mykaku.h>
#include <mymqtt.h>
#include <Ticker.h>

Ticker ledflash;

/**
 * Is called when a valid KaKu command is received over 433 Mhz
  */
void kakuReceived(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
  PRINTLN("KAKU command received from ", sender)
  mqtt_kakucmd(sender, groupBit, unit, switchType);
  if (switchType == 1) {
    mqtt_setStatus(cmdOn);
  } else {
    mqtt_setStatus(cmdOff);
  }
}

/**
 * Is called when a command is received over the MQTT command topic
 */
void MQTTCmdReceived(const char* cmd) {
  PRINTLN("MQTT command received: ", cmd);
  if (strcmp(cmdOn, cmd) == 0) {
    kaku_setLightsOn();
    mqtt_setStatus(cmdOn);
  } else {
    kaku_setLightsOff();
    mqtt_setStatus(cmdOff);
  }
}

/**
 * ISR to toggle the builtin LED for status info
 */
void flashLed() {
  int state = digitalRead(LED_BUILTIN);
  digitalWrite(LED_BUILTIN, !state);  
}

void setup() {
  DEBUGSTARTDEF
  
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, HIGH);
  
  ledflash.attach(0.3, flashLed);

  kaku_start(kakuReceived);
  mqtt_start(MQTTCmdReceived);

  ledflash.detach();
}

void loop() {
  mqtt_loop();
}