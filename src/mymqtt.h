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
#include <map>
#ifndef HASS_ENTITYNAME
  #define HASS_ENTITYNAME livingroom-lights
#endif

#ifdef TESTCMD
  #define TOPIC_CMD "cmdtest"
#else
  #define TOPIC_CMD "cmd"
#endif
#define TOPIC_RESULT "result"
#define TOPIC_STATUS "status"
#define TOPIC_LWT "lwt"
#define TOPIC_KAKU "kaku"
#define TOPIC_KAKU_REJECTED "kakureject"
#define VAL_ONLINE "online"
#define VAL_OFFLINE "offline"
#define VAL_PL_ON "ON"
#define VAL_PL_OFF "OFF"
#define VAL_PL_OTAON "OTAON"
#define VAL_PL_OTAOFF "OTAOFF"

#ifdef HASS_AUTODISCOVERY
enum ADCATEGORY {
  none,
  diagnostic,
  config
};

struct ADInfo {
  String component;
  String node;
  ADCATEGORY category;
  String stateTopic;
  String valtemplate;
  String name;
  String objectID;
  String uniqueID;
  bool availability;
  String availabilityTopic;
  String payloadAvailable;
  String payloadNotAvailable;
  String payloadOn;
  String payloadOff;
  String deviceClass;
  bool enabledDefault;
  String icon;
  String attrTopic;
  String attrTempl;
  /*
    device_class
    enabled_by_default
    icon
    "json_attributes_template": "{{ value_json.data.value | tojson }}",
    "json_attributes_topic": "zigbee2mqtt/bridge/response/networkmap",
  */
  ADInfo() : 
    category(none), 
    availability(true), 
    enabledDefault(false) {};
};
#endif // HASS_AUTODISCOVERY
typedef std::function<void(const char* cmd)> MqttCmdReceived;

String mainTopic;
String cmdTopic;
String statusTopic;
String willTopic;
String kakuTopic;
String rejectedTopic;
String resultTopic;

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
  rejectedTopic = mainTopic  + "/" + TOPIC_KAKU_REJECTED;
  resultTopic = mainTopic + "/" + TOPIC_RESULT;

  PRINTLN("Main topic: ", mainTopic)
  PRINTLN("Command topic: ", cmdTopic)
  PRINTLN("Status topic: ", statusTopic)
  PRINTLN("Will topic: ", willTopic)
  PRINTLN("Kaku topic: ", kakuTopic)
  PRINTLN("Rejected topic: ", rejectedTopic)
  PRINTLN("Result topic: ", resultTopic)

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



std::map<ADCATEGORY, String> adCategory {
  {none, "none"},
  {diagnostic, "diagnostic"},
  {config, "config"}
};

String strIfNull(String val, String alt) {
  return val.isEmpty()?alt:val;
}

int8_t postADinfo(ADInfo* info) {
  // For available entries see: https://www.home-assistant.io/integrations/mqtt/#discovery-payload
  assert(!info->component.isEmpty());
  assert(!info->node.isEmpty());
  assert(!info->stateTopic.isEmpty());
  
  String path = "homeassistant/" + info->component + "/homelight_-" + _getId() + "/" + info->node + "/config";
  
  JsonDocument doc;
  doc["~"] = mainTopic;
  doc["name"] = strIfNull(info->name, info->node);
  doc["stat_t"] = info->stateTopic;
  if (!info->valtemplate.isEmpty()) doc["val_tpl"] = "{{ " + info->valtemplate + " }}";
  if(!info->payloadOn.isEmpty()) doc["pl_on"] = info->payloadOn;
  if(!info->payloadOff.isEmpty()) doc["pl_off"] = info->payloadOff;
  
  doc["uniq_id"] = strIfNull(info->uniqueID, STRING(HASS_ENTITYNAME) + "_" + _getId());
  doc["obj_id"] = strIfNull(info->objectID, doc["uniq_id"]);

  // if (!info->attrTempl.isEmpty()) {
  //   // _rootTopic + PATHSEP
  //   doc["json_attr_tpl"] = info->attrTempl;
  //   doc["json_attr_t"] = strIfNull(info->attrTopic, mainTopic + "/" + MQTT_ATTR_TOPIC);
  // }

  if (info->category>none) doc["ent_cat"] = adCategory[info->category];

  if (info->availability) {
    doc["avty_t"] = strIfNull(info->availabilityTopic, willTopic.c_str());
    doc["pl_avail"] = strIfNull(info->payloadAvailable, VAL_ONLINE);
    doc["pl_not_avail"] = strIfNull(info->payloadNotAvailable, VAL_OFFLINE);
  }

  JsonObject dev = doc["device"].to<JsonObject>();
  dev["hw"] = QUOTE(FIRMWARE_NAME PLATFORM);
  dev["ids"] = STRING(HASS_ENTITYNAME) + "_device_" + _getId();
  dev["mf"] = "Joost Bloemsma";
  dev["mdl"] = QUOTE(HASS_FRIENDLYNAME);
  dev["name"] = QUOTE(HASS_FRIENDLYNAME);
  dev["sw"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);
  
  JsonObject origin = doc["origin"].to<JsonObject>();
  origin["name"] = QUOTE(HASS_FRIENDLYNAME);
  origin["sw"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);

  String json;

  serializeJson(doc, json);
  //return publish(path, json, true, 1);
  return mqttClient.publish(path.c_str(), json.c_str(), true);
}

void _mqtt_config_hassdiscovery() {
}

void _mqtt_config_hassdiscoverys() {
  PRINTLNS("Configuring HASS autodiscovery")
  String hassName = String(QUOTE(HASS_ENTITYNAME));
  String hasstopic = "homeassistant/switch/" + hassName + "/config";
  DynamicJsonDocument doc(2048);
  doc["~"] = mainTopic;
  doc["avty_t"] = willTopic.c_str();
  doc["name"] = QUOTE(HASS_FRIENDLYNAME);
  doc["stat_t"] = "~/" + String(TOPIC_STATUS);
  doc["cmd_t"] = "~/" + String(TOPIC_CMD);
  doc["ic"] = "mdi:lightbulb";
  doc["uniq_id"] = hassName + "_" + _getId();
  doc["pl_avail"] = VAL_ONLINE;
  doc["pl_not_avail"] = VAL_OFFLINE;
  doc["pl_on"] = VAL_PL_ON;
  doc["pl_off"] = VAL_PL_OFF;
  doc["pl_avail"] = VAL_ONLINE;
  doc["device"]["manufacturer"] = "Joost Bloemsma";
  doc["device"]["model"] = "1";
  doc["device"]["ids"] = hassName + "_device_" + _getId();
  doc["device"]["name"] = QUOTE(FIRMWARE_NAME);
  //doc["device"]["fw_version"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);
  //doc["device"]["sw_version"] = QUOTE(FIRMWARE_VERSION);
  doc["device"]["hw"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);
  doc["device"]["sw"] = QUOTE(FIRMWARE_VERSION);

  doc.shrinkToFit();

  String output;
  serializeJson(doc, output);

  mqttClient.publish(hasstopic.c_str(), output.c_str(), true);

  #ifdef ARDUINO_OTA
  
  doc.clear();

  doc["~"] = mainTopic;
  doc["avty_t"] = willTopic.c_str();
  doc["name"] = String(QUOTE(HASS_FRIENDLYNAME)) + " (OTA)";
  doc["stat_t"] = "~/" + String(TOPIC_OTA);
  doc["cmd_t"] = "~/" + String(TOPIC_CMD);
  doc["ic"] = "mdi:lightbulb";
  doc["uniq_id"] = hassName + "_ota_" + _getId();
  doc["pl_avail"] = VAL_ONLINE;
  doc["pl_not_avail"] = VAL_OFFLINE;
  doc["pl_on"] = VAL_PL_OTAON;
  doc["pl_off"] = VAL_PL_OTAOFF;
  doc["device"]["manufacturer"] = "Joost Bloemsma";
  doc["device"]["model"] = "1";
  doc["device"]["ids"] = hassName + "_device_" + _getId();
  doc["device"]["name"] = QUOTE(FIRMWARE_NAME);
  //doc["device"]["fw_version"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);
  //doc["device"]["sw_version"] = QUOTE(FIRMWARE_VERSION);
  doc["device"]["hw"] = QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM);
  doc["device"]["sw"] = QUOTE(FIRMWARE_VERSION);

  doc.shrinkToFit();

  output = "";
  serializeJson(doc, output);
  String hassOtaTopic = "homeassistant/switch/" + hassName + "_ota/config";
  mqttClient.publish(hassOtaTopic.c_str(), output.c_str(), true);

  #endif

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
      VAL_OFFLINE
    );
    if (rc) {
      PRINTLNS("connected")
      #ifdef HASS_AUTODISCOVERY
      _mqtt_config_hassdiscovery();
      #endif
      mqttClient.publish((mainTopic + "/ip").c_str(), WiFi.localIP().toString().c_str(), true);
      mqttClient.publish((mainTopic + "/fw_version").c_str(), QUOTE(FIRMWARE_VERSION), true);
      mqttClient.publish((mainTopic + "/fw_name").c_str(), QUOTE(FIRMWARE_NAME FIRMWARE_VERSION PLATFORM),true);

      mqttClient.publish(willTopic.c_str(), VAL_ONLINE, true);
      
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

void mqtt_showResult(const char* result) {
  if (!mqttClient.connected()) {
    _mqtt_reconnect();
  }
  mqttClient.publish(resultTopic.c_str(), result, false);
  
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
 * @brief Published the last REJECTED KAKU command to the KAKU_REJECTED topic
 * 
 * @param sender 
 * @param groupBit 
 * @param unit 
 * @param switchType 
 */
void mqtt_kakurejected(unsigned long sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
  DynamicJsonDocument doc(1024);
  doc["sender"] = sender;
  doc["groupBit"] = groupBit;
  doc["unit"] = unit;
  doc["switchType"] = switchType;

  String output;
  serializeJson(doc, output);

  mqttClient.publish(rejectedTopic.c_str(), output.c_str(), true);
}

/**
 * @brief Initializes the MQTT client
 * 
 * @param mqttCmdReceived 
 */
void mqtt_start(MqttCmdReceived mqttCmdReceived) {
  _mqttCmdReceived = mqttCmdReceived;
  _mqtt_setTopics();
  mqttClient.setBufferSize(2048,2048);
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