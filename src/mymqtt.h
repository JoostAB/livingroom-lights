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
#ifdef ARDUINO_OTA
#define TOPIC_OTA "ota"
String otaTopic;
#endif

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

MqttCmdReceived _mqttCmdReceived;


/**
 * @brief Sets the mqtt topics based on main topic from wifi configuration
 * 
 */
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

  #ifdef ARDUINO_OTA
  otaTopic = mainTopic  + "/" + TOPIC_OTA;
  PRINTLN("OTA topic: ", otaTopic)
  #endif
}

String _getId() {
  uint32_t chipId;
  #ifdef ESP32
    // see:
    // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ChipID/GetChipID/GetChipID.ino
    // https://forum.arduino.cc/t/esp8266-style-chipid-for-esp32/589349/3
    for(int i=0; i<17; i=i+8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    /*
    Serial.printf("%08X\n", id);
    */
    
  #else
    chipId = ESP.getChipId();
  #endif
  return String(chipId);
}
/**
 * @brief Called each time an MQTT message comes in through one of
 *        the subscribed topics
 * 
 * This will call the MqttCmdReceived callback function registered
 * when calling mqtt_start.
 * 
 * @param topic The topic the message comes from
 * @param payload Payload of the message
 * @param length Length in bytes
 */
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
/**
 * @brief Generates and published the configuration for HASS to enable autodiscovery.
 * 
 * A unique ID is generated from entity name and esp chip id.
 * 
 */
void _mqtt_config_hassdiscovery() {
  PRINTLNS("Configuring HASS autodiscovery")
  String hasstopic = "homeassistant/switch/" + String(HASS_ENTITYNAME) + "/config";
  DynamicJsonDocument doc(1024);
  doc["~"] = mainTopic;
  doc["name"] = HASS_ENTITYNAME;
  doc["stat_t"] = "~/" + String(TOPIC_STATUS);
  doc["cmd_t"] = "~/" + String(TOPIC_CMD);
  doc["ic"] = "mdi:lightbulb";
  doc["uniq_id"] = String(HASS_ENTITYNAME) + "_" + _getId();

  String output;
  serializeJson(doc, output);

  mqttClient.publish(hasstopic.c_str(), output.c_str(), true);

  PRINTLNS("Configured HASS Autodiscovery: ")
  PRINTLNSA(output)

}
#endif

/**
 * @brief Will (re) connect to the mqtt broker. If it fails 5 successive times the 
 * whole chip is rebooted to start the configuration portal
 */
void _mqtt_reconnect() {
  int tryCount = 0;
  while (!mqttClient.connected() && (tryCount < 5)) {
    PRINTLN("Attempting MQTT connection to ", wifi_get_mqttServer())
    String clientId = "homelight-livingroom-" + _getId();
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
      mqttClient.publish((mainTopic + "/ip").c_str(), WiFi.localIP().toString().c_str(), true);
      mqttClient.publish(willTopic.c_str(), "online", true);
      mqttClient.publish((mainTopic + "/fw_name").c_str(), QUOTE(FIRMWARE_NAME), true);
      mqttClient.publish((mainTopic + "/fw_version").c_str(), QUOTE(FIRMWARE_VERSION), true);
      
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


/**
 * @brief Published the current status (ON or OFF) to the status topic
 * 
 * @param status 
 */
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

#ifdef ARDUINO_OTA
void mqtt_setOtaStatus(const char* status) {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.publish(otaTopic.c_str(), status, true);
}
#endif

/**
 * @brief Published the last KAKU command to the KAKU topic
 * 
 * @param sender 
 * @param groupBit 
 * @param unit 
 * @param switchType 
 */
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

/**
 * @brief Initializes the MQTT client
 * 
 * @param mqttCmdReceived 
 */
void mqtt_start(MqttCmdReceived mqttCmdReceived) {
  _mqttCmdReceived = mqttCmdReceived;
  _mqtt_setTopics();
  mqttClient.setServer(wifi_get_mqttServer(), wifi_get_mqttPort());
  mqttClient.setCallback(_mqtt_callback);
}

/**
 * @brief To be called as often as possible from main loop.
 * If not connected calls _mqtt_reconnect.
 * 
 */
void mqtt_loop() {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.loop();
}

#endif // __MY_MQTT_H__