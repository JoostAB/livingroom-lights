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

#include <general.h>
#include <mywifi.h>

#include <ArduinoJson.h>
#include <PubSubClient.h>

#define HASS_AUTODISCOVERY
#define HASS_ENTITYNAME "Woonkamerlicht"

#ifdef TESTCMD
  #define TOPIC_CMD "cmdtest"
#else
  #define TOPIC_CMD "cmd"
#endif
#define TOPIC_STATUS "status"
#define TOPIC_LWT "lwt"
#define TOPIC_KAKU "kaku"

typedef std::function<void(const char* cmd)> MqttCmdReceived;

String mainTopic;
String cmdTopic;
String statusTopic;
String willTopic;
String kakuTopic;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

MqttCmdReceived _mqttCmdReceived;

void _mqtt_setTopics() {
  mainTopic = wifi_get_mqttTopic();
  cmdTopic = mainTopic  + "/" + TOPIC_CMD;
  statusTopic = mainTopic  + "/" + TOPIC_STATUS;
  willTopic = mainTopic  + "/" + TOPIC_LWT;
  kakuTopic = mainTopic  + "/" + TOPIC_KAKU;

  PRINTLN("Main topic: ", mainTopic)
  PRINTLN("Command topic: ", cmdTopic)
  PRINTLN("Status topic: ", statusTopic)
  PRINTLN("Will topic: ", willTopic)
  PRINTLN("Kaku topic: ", kakuTopic)
}

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

#ifdef HASS_AUTODISCOVERY
void _mqtt_config_hassdiscovery() {
  PRINTLNS("Configuring HASS autodiscovery")
  String hasstopic = "homeassistant/switch/" + String(HASS_ENTITYNAME) + "/config";
  DynamicJsonDocument doc(1024);
  doc["~"] = mainTopic;
  doc["name"] = HASS_ENTITYNAME;
  doc["stat_t"] = "~/" + String(TOPIC_STATUS);
  doc["cmd_t"] = "~/" + String(TOPIC_CMD);
  doc["ic"] = "mdi:lightbulb";
  doc["uniq_id"] = String(HASS_ENTITYNAME) + "_" + String(ESP.getChipId());

  String output;
  serializeJson(doc, output);

  PRINTLNS("configuring HASS Autodiscovery: ")
  PRINTLNSA(output)

  mqttClient.publish(hasstopic.c_str(), output.c_str(), true);
}
#endif

void _mqtt_reconnect() {
  int tryCount = 0;
  while (!mqttClient.connected() && (tryCount < 5)) {
    PRINTLN("Attempting MQTT connection to ", wifi_get_mqttServer())
    String clientId = "homelight-livingroom-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    bool rc = mqttClient.connect(
      clientId.c_str(),
      wifi_get_mqttUser(),
      wifi_get_mqttPassword(),
      willTopic.c_str(),
      0,
      true,
      "offline"
    );
    if (rc) {
      PRINTLNS("connected")
      #ifdef HASS_AUTODISCOVERY
      _mqtt_config_hassdiscovery();
      #endif
      mqttClient.publish((mainTopic + "ip").c_str(), WiFi.localIP().toString().c_str(), true);
      mqttClient.publish(willTopic.c_str(), "online", true);
      // ... and resubscribe
      mqttClient.subscribe(cmdTopic.c_str());
      return;
    } else {
      PRINTS("failed, rc=")
      PRINTDS(mqttClient.state())
      PRINTLNS(" try again in 5 seconds")
      // Wait 5 seconds before retrying
      delay(5000);
      tryCount ++;
    }
  }
  PRINTLNS("Connecting not possible")
  wifi_cleanStart();
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

void mqtt_start(MqttCmdReceived mqttCmdReceived) {
  _mqttCmdReceived = mqttCmdReceived;
  _mqtt_setTopics();
  mqttClient.setServer(wifi_get_mqttServer(), wifi_get_mqttPort());
  mqttClient.setCallback(_mqtt_callback);
}

void mqtt_loop() {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.loop();
}

#endif // __MY_MQTT_H__