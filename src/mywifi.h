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

#include <Arduino.h>
// #include <FS.h>
// #ifdef ESP32
//   #include <SPIFFS.h>
// #endif
#include <LittleFS.h>

#include <jbdebug.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
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

char* wifi_get_mqttServer() { return _mqtt_server; }
int wifi_get_mqttPort() { return atoi(_mqtt_port); }
char* wifi_get_mqttUser() { return _mqtt_user; }
char* wifi_get_mqttPassword() { return _mqtt_password; }
char* wifi_get_mqttTopic() { return _mqtt_topic; }

/**
 * @brief Starts the WifiManager configuration portal on own AP
 * 
 */
void wifi_start_configPortal() {
  wifiManager.startConfigPortal(PORTAL_AP_NAME);
}

/**
 * @brief Starts the WifiManager configuration page
 * 
 */
void wifi_start_configWeb() {
  wifiManager.startWebPortal();
}

void _wifi_saveCfgCb () {
  PRINTLNS("Should save config")
  _saveCfg = true;
}

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


bool _wifi_cfg_read() {
  PRINTLNS("Reading configuration from filesystem.")

  if (!_wifi_fsBegin()) return false;

  File cfgFile = LittleFS.open(cfgFilename, "r");
  if (cfgFile) {
    PRINTLN("Reading config file: ", cfgFilename);
    size_t size = cfgFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    cfgFile.readBytes(buf.get(), size);
    
    PRINTLNS("Parsing file to JSON")
    DynamicJsonDocument json(1024);
    auto deserializeError = deserializeJson(json, buf.get());
    
    if (!deserializeError ) {  
      PRINTLNS("File read succesfull.")
      IFDEBUG(
        String _tmpJson;
        serializeJsonPretty(json, _tmpJson);
        PRINTLNSA(_tmpJson)
      )
      strcpy(_mqtt_server, json["mqtt_server"]);
      strcpy(_mqtt_port, json["mqtt_port"]);
      strcpy(_mqtt_user, json["mqtt_user"]);
      strcpy(_mqtt_password, json["mqtt_password"]);
      strcpy(_mqtt_topic, json["mqtt_topic"]);
    } else {
      PRINTLN("Failed to parse ", cfgFilename)
      return false;
    }
    cfgFile.close();
  } else {
    PRINTLN("Failed to open ", cfgFilename)
    return false;
  }
  return true;
}

bool _wifi_cfg_write() {
  PRINTLNS("Writing configuration to filesystem.")

  if (!_wifi_fsBegin()) return false;

  DynamicJsonDocument json(1024);
      
  json["mqtt_server"] = _mqtt_server;
  json["mqtt_port"] = _mqtt_port;
  json["mqtt_user"] = _mqtt_user;
  json["mqtt_password"] = _mqtt_password;
  json["mqtt_topic"] = _mqtt_topic;

  File cfgFile = LittleFS.open(cfgFilename,"w");

  if (!cfgFile) {
    PRINTLN("Error opening file ", cfgFilename)
    return false;
  }

  size_t size = serializeJson(json, cfgFile);
  cfgFile.close();

  if (size > 0) {
    PRINTF2("Written %u bytes to %s.", size, cfgFilename)
    PRINTLF
    return true;
  }

  PRINTLN("Nothing written to ", cfgFilename)
  return false;
}

/**
 * @brief Connect to wifi using WiFiManager
 * 
 */
bool wifi_connect() {
  
  _wifi_cfg_read();

  // File read (or not)
  WiFiManagerParameter custom_mqtt_server("mqttserver", "MQTT host", _mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("mqttport", "MQTT port", _mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("mqttuser", "User", _mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("mqttpassword", "Passw.", _mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_topic("mqtttopic", "Topic", _mqtt_topic, 40);
  
  
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
      strcpy(_mqtt_server, custom_mqtt_server.getValue());
      strcpy(_mqtt_port, custom_mqtt_port.getValue());
      strcpy(_mqtt_user, custom_mqtt_user.getValue());
      strcpy(_mqtt_password, custom_mqtt_password.getValue());
      strcpy(_mqtt_topic, custom_mqtt_topic.getValue());

      _wifi_cfg_write();
    }
    return true;
  } else {
    PRINTLNS("Not connected to wifi...!")
  }
  return false;
}
#endif // __MY_WIFI_H__