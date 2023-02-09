/**
 * @file mymqtt.h
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-09
 */
# pragma once
#ifndef __MY_MQTT_H__
#define __MY_MQTT_H__

#include <Arduino.h>
#include <jbdebug.h>
#include <ArduinoJson.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <WiFiManager.h>
#include <PubSubClient.h>

#define PORTAL_AP_NAME "HomeLight-config"

typedef std::function<void(const char* cmd)> MqttCmdReceived;

const String mainTopic = "homelight/livingroom/main/";
const String cmdTopic = mainTopic + "cmd";
const String statusTopic = mainTopic + "status";
const String willTopic = mainTopic + "lwt";
const String kakuTopic = mainTopic + "kaku";

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

MqttCmdReceived _mqttCmdReceived;

void _mqtt_callback(char* topic, byte* payload, unsigned int length) {
  IFDEBUG(
    PRINTLNS("Received mqtt command :")
    PRINTLNSA(topic)
  
    for (u_int i = 0; i < length; i++) {
      PRINTDS((char)payload[i]);
    }
    PRINTLF
    PRINTLN("Length of payload: ", length)
  )
  
  char chars[length + 1];
  memcpy(chars, payload, length);
  chars[length] = '\0';
  _mqttCmdReceived(chars);
}

void _mqtt_reconnect() {
  while (!mqttClient.connected()) {
    PRINTLNS("Attempting MQTT connection...")
    String clientId = "homelight-livingroom-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    bool rc = mqttClient.connect(
      clientId.c_str(),
      "mqtt",
      "mqtt_user",
      willTopic.c_str(),
      0,
      true,
      "offline"
    );
    if (rc) {
      PRINTLNS("connected")
      mqttClient.publish((mainTopic + "ip").c_str(), WiFi.localIP().toString().c_str(), true);
      mqttClient.publish(willTopic.c_str(), "online", true);
      // ... and resubscribe
      mqttClient.subscribe(cmdTopic.c_str());
    } else {
      PRINTS("failed, rc=")
      PRINTDS(mqttClient.state())
      PRINTLNS(" try again in 5 seconds")
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_setStatus(const char* status) {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.publish(statusTopic.c_str(), status, true);
  IFDEBUG(
    if (strcmp(cmdOn, status) == 0) {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  )
}

void mqtt_start_configPortal() {
  wifiManager.startConfigPortal(PORTAL_AP_NAME);
}

void mqtt_start_configWeb() {
  wifiManager.startWebPortal();
}

void mqtt_kakucmd(unsigned long sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
  DynamicJsonDocument doc(1024);
  doc["sender"] = sender;
  doc["groupBit"] = groupBit;
  doc["unit"] = unit;
  doc["switchType"] = switchType;

  String output;
  serializeJson(doc, output);

  mqttClient.publish(kakuTopic.c_str(), output.c_str(), true);
}

// void _mqtt_wifiManConfigModeCallback (WiFiManager *myWiFiManager) {
//   PRINTLN("Entering Config mode at ", myWiFiManager->getConfigPortalSSID())
//   PRINTLN("IP address: ", WiFi.softAPIP())
// }

void mqtt_start(MqttCmdReceived mqttCmdReceived) {
  _mqttCmdReceived = mqttCmdReceived;
  
  //wifiManager.setAPCallback(_mqtt_wifiManConfigModeCallback);
  wifiManager.autoConnect(PORTAL_AP_NAME);

  PRINTLN("WiFi connected with IP address: ", WiFi.localIP())

  mqttClient.setServer("192.168.0.10", 1883);
  mqttClient.setCallback(_mqtt_callback);
}

void mqtt_loop() {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.loop();
}

#endif // __MY_MQTT_H__