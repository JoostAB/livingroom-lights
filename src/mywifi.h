/**
 * @file mywifi.h
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-09
 */
# pragma once
#ifndef __MY_WIFI_H__
#define __MY_WIFI_H__

#include <general.h>
#include <LittleFS.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#ifdef ARDUINO_OTA
#include <ArduinoOTA.h>
bool OTArunning = false;
#endif

#include <ArduinoJson.h>
#include <WiFiManager.h>

#define PORTAL_AP_NAME "HomeLight-config"

WiFiManager wifiManager;

char _mqtt_server[40];
char _mqtt_port[6] = "1883";
char _mqtt_user[40];
char _mqtt_password[40];
char _mqtt_topic[40] = "homelight/livingroom/main/";

const char* cfgFilename = "/wificfg.json";

bool _saveCfg = false;
bool _fsBegun = false;

/**
 * @brief Returns hostname of the mqtt server, as registered in config portal
 * 
 * @return char* 
 */
char* wifi_get_mqttServer() { return _mqtt_server; }

/**
 * @brief Returns port of the mqtt server, as registered in config portal
 * 
 * @return int 
 */
int wifi_get_mqttPort() { return atoi(_mqtt_port); }

/**
 * @brief Returns user of the mqtt server, as registered in config portal
 * 
 * @return char* 
 */
char* wifi_get_mqttUser() { return _mqtt_user; }

/**
 * @brief Returns user password of the mqtt server, as registered in config portal
 * 
 * @return char* 
 */
char* wifi_get_mqttPassword() { return _mqtt_password; }

/**
 * @brief Returns main mqtt topic, as registered in config portal. If the topic ends with 
 * a forward slash, it is stripped
 * 
 * @return char* 
 */
char* wifi_get_mqttTopic() { 
  size_t ln = strlen(_mqtt_topic);
  if (_mqtt_topic[ln - 1] == '/') {
    PRINTLNS("MQTT topic ends with /, stripping off")
    _mqtt_topic[ln - 1] = '\0';
  }
  return _mqtt_topic; 
}

/**
 * @brief Starts the WifiManager configuration portal on own AP
 * 
 */
void wifi_start_configPortal() {
  wifiManager.startConfigPortal(PORTAL_AP_NAME);
}

/**
 * @brief Clears all WiFiManager settings and reboots
 * 
 */
void wifi_cleanStart() {
  PRINTLNS("Clearing settings and restart")
  wifiManager.resetSettings();
  delay(2000);
  ESP.restart();
}

/**
 * @brief Starts the WifiManager configuration page
 * 
 */
void wifi_start_configWeb() {
  wifiManager.startWebPortal();
}

/**
 * @brief Set the saveCfg flag to true
 * 
 */
void _wifi_saveCfgCb () {
  PRINTLNS("Should save config")
  _saveCfg = true;
}

/**
 * @brief Initializes filesystem is nessecary
 * 
 * @return true 
 * @return false 
 */
bool _wifi_fsBegin() {
  if (_fsBegun) return true;
  PRINTLNS("Mounting filesystem for parameters")
  if (!LittleFS.begin()) {
    PRINTLNS("Failed to mount filesystem")
    return false;
  }
  PRINTLNS("Filesystem mounted")
  _fsBegun = true;
  return true;
}

#ifdef ARDUINO_OTA
void wifi_start_OTA() {
  PRINTLNS("Setting up OTA")
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    PRINTLN("Start updating ", type)
  });
  ArduinoOTA.onEnd([]() {
    PRINTLNS("OTA end")
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int p = (progress / (total / 100));
    char pb[51] = "==================================================";
    pb[p/2] = '\0';
    PRINT("Progress: ", pb)
    PRINTF(" %u precent", p)
    PRINTLF
    //PRINTF("Progress: %u%%\r", (progress / (total / 100)))
    //PRINTDS(".")
  });
  ArduinoOTA.onError([](ota_error_t error) {
    PRINTF("Error[%u]: \n", error)

    if (error == OTA_AUTH_ERROR) {
      PRINTLNS("Auth Failed")
    } else if (error == OTA_BEGIN_ERROR) {
      PRINTLNS("Begin Failed")
    } else if (error == OTA_CONNECT_ERROR) {
      PRINTLNS("Connect Failed")
    } else if (error == OTA_RECEIVE_ERROR) {
      PRINTLNS("Receive Failed")
    } else if (error == OTA_END_ERROR) {
      PRINTLNS("End Failed")
    }
  });
  ArduinoOTA.begin();
  OTArunning = true;
}

void wifi_stop_OTA() {
  PRINTLNS("Stopping OTA")
  ArduinoOTA.end();
  OTArunning = false;
}

bool wifi_is_ota_on() {
  return OTArunning;
}
#else
// Dummy stub methods 
void wifi_start_OTA() {}
void wifi_stop_OTA() {}
bool wifi_is_ota_on() {{ return false; }
#endif

/**
 * @brief Connect to wifi using WiFiManager
 * 
 * @return true if success
 * @return false if failure
 */
bool wifi_start(MyConfig* _cfg) {
  
  char port[6];
  char server[40];
  char user[40];
  char pwd[40];
  char topic[40];

  _cfg->getMqttCfg(port, server, user, pwd, topic);

  WiFiManagerParameter custom_mqtt_server("mqttserver", "MQTT host", server, 40);
  WiFiManagerParameter custom_mqtt_port("mqttport", "MQTT port", port, 6);
  WiFiManagerParameter custom_mqtt_user("mqttuser", "User", user, 40);
  WiFiManagerParameter custom_mqtt_password("mqttpassword", "Passw.", pwd, 40);
  WiFiManagerParameter custom_mqtt_topic("mqtttopic", "Topic", topic, 40);
  
  wifiManager.setSaveConfigCallback(_wifi_saveCfgCb);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_topic);

  if (wifiManager.autoConnect(PORTAL_AP_NAME)) {
    if (_saveCfg) {
      //read updated parameters
      PRINTLNS("New configuration, parsing returned data")
      _cfg->setMqttServer(custom_mqtt_server.getValue());
      _cfg->setMqttPort(custom_mqtt_port.getValue());
      _cfg->setMqttUser(custom_mqtt_user.getValue());
      _cfg->setMqttPassword(custom_mqtt_password.getValue());
      _cfg->setMqttTopic(custom_mqtt_topic.getValue());

      _cfg->saveConfig();
    }

    #if defined(ARDUINO_OTA) && defined(ARDUINO_OTA_ALWAYS)
    wifi_start_OTA();
    #endif
    return true;
  } else {
    PRINTLNS("Not connected to wifi...!")
  }
  return false;
}

/**
 * @brief To be called as often as possible in main loop
 * 
 */
void wifi_loop() {
  #ifdef ARDUINO_OTA
  if (OTArunning) ArduinoOTA.handle();
  #endif
}
#endif // __MY_WIFI_H__