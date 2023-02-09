/**
 * @file main.cpp
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-09
 */
#include <Arduino.h>

#define DEBUGLOG 1

const char* cmdOn = "ON";
const char* cmdOff = "OFF";
const char* cmdReCfg = "RECFG";
const char* cmdWebCfg = "WEBCFG";

#include <jbdebug.h>
#include <mykaku.h>
#include <mymqtt.h>
#include <Ticker.h>

Ticker ledflash;

/**
 * ISR to toggle the builtin LED for status info
 */
void flashLed() {
  int state = digitalRead(LED_BUILTIN);
  digitalWrite(LED_BUILTIN, !state);  
}

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
  } else if (strcmp(cmdOff, cmd) == 0) {
    kaku_setLightsOff();
    mqtt_setStatus(cmdOff);
  } else if (strcmp(cmdReCfg, cmd) == 0) {
    ledflash.attach(0.3, flashLed);
    mqtt_start_configPortal();
    ledflash.detach();
  } else if (strcmp(cmdWebCfg, cmd) == 0) {
    ledflash.attach(0.3, flashLed);
    mqtt_start_configWeb();
    ledflash.detach();
  } else {
    PRINTLN("Unknown command received: ", cmd)
  }
}

void setup() {
  DEBUGSTARTDEF
  
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, HIGH);
  
  ledflash.attach(0.3, flashLed);

  mqtt_start(MQTTCmdReceived);
  kaku_start(kakuReceived);
  
  ledflash.detach();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  mqtt_loop();
}