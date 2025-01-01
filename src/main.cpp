/**
 * @file main.cpp
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-09
 */
#include <main.h>

/**
 * ISR to toggle the builtin LED for status info
 */
void _flashLed_isr() {
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
    ledflash.attach(0.3, _flashLed_isr);
    wifi_start_configPortal();
    ledflash.detach();
  } else if (strcmp(cmdWebCfg, cmd) == 0) {
    ledflash.attach(0.3, _flashLed_isr);
    wifi_start_configWeb();
    ledflash.detach();
  #ifdef ARDUINO_OTA
  } else if (strcmp(cmdOtaOn, cmd) == 0) {
    wifi_start_OTA();
    mqtt_setOtaStatus(cmdOn);
    ledflash.attach(1.0, _flashLed_isr);
  } else if (strcmp(cmdOtaOff, cmd) == 0) {
    wifi_stop_OTA();
    mqtt_setOtaStatus(cmdOff);
    ledflash.detach();
  #endif
  } else if (strcmp(cmdReboot, cmd) == 0) {
    PRINTLNS("Reboot requested over MQTT...")
    ESP.restart();
  } else {
    PRINTLN("Unknown command received: ", cmd)
  }
}

void setup() {
  DEBUGSTARTDEF
  
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, HIGH);
  
  ledflash.attach(0.3, _flashLed_isr);

  if (!wifi_start()) {
    // Failed starting wifi. No point in continuing so restart
    PRINTLNS("Rebooting...");
    delay(500);
    ESP.restart();
  }
  mqtt_start(MQTTCmdReceived);
  kaku_start(kakuReceived);
  webui_start();

  ledflash.detach();
  digitalWrite(LED_BUILTIN, HIGH);

  #ifdef ARDUINO_OTA
  if (wifi_is_ota_on()) {
    mqtt_setOtaStatus(cmdOn);
  } else {
    mqtt_setOtaStatus(cmdOff);
  }
  #endif
}

void loop() {
  mqtt_loop();
  wifi_loop();
  webui_loop();
}