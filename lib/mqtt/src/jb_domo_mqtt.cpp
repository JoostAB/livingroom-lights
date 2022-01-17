/**
 * @file jb_domo_mqtt.cpp
 * @author Joost Bloemsma (joost.a@bloemsma.net)
 * @brief 
 * @version 1.0
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <jb_domo_mqtt.h>

/** Constructors **/

DomoMQTTClient::DomoMQTTClient(String host, uint16_t port = (uint16_t)1883U) {
  DomoMQTTClient(host, "", port);
}

DomoMQTTClient::DomoMQTTClient(String host, String rootTopic, uint16_t port = (uint16_t)1883U) {
  DomoMQTTClient(host, rootTopic, "cmd", "status", "lwt", "ON", "OFF", "Online", "Offline", port);
}

DomoMQTTClient::DomoMQTTClient(String host, String rootTopic, String cmdTopic = "cmd", String statusTopic = "status", 
        String lwtTopic = "lwt", String onCmd = "ON", String offCmd = "OFF", String onlineLwt = "Online",
        String offlineLwt = "Offline", uint16_t port = 1883) {
  _host = host;
  _port = port;

  /* TODO: fill with useful stuff */
  _mqttId = "something";
  _mqttUser = "mqtt";
  _mqttPwd = "mqtt_user";

  try
  {
    setRootTopic(rootTopic);
  }
  catch(const std::exception& e)
  {
    return;
  }
    
  setCmdTopic(_rootTopic + "\\" + cmdTopic);
  setstatusTopic(_rootTopic + "\\" + statusTopic);
  setLwtTopic(_rootTopic + "\\" + lwtTopic);
  setOnCmd(onCmd);
  setOffCmd(offCmd);
  setOnlineLwt(onlineLwt);
  setOfflineLwt(offlineLwt);
}

success_t DomoMQTTClient::start() {
  WFClient wifiClient;
  unsigned long connectStart = millis();
  
  #ifdef JBDOMODEBUG
  Serial.print("connecting to mqtt broker host...");
  #endif

  while ((!wifiClient.connect(_host, _port)) && (millis() - connectStart < 5000)) {
    #ifdef JBDOMODEBUG
    Serial.print(".");
    #endif
    delay(200);
  }
  if (!wifiClient.connected()) {
    #ifdef JBDOMODEBUG
    Serial.println("Failed!");
    #endif
    return -1;
  }
  #ifdef JBDOMODEBUG
  Serial.println("Success");
  #endif

  _wifiClient = &wifiClient;
  return start(&wifiClient);
}

success_t DomoMQTTClient::start(WFClient* wifiClient) {
  MQTTPubSubClient mqttClient;
  unsigned long connectStart = millis();

  mqttClient.begin(*wifiClient);

  #ifdef JBDOMODEBUG
  Serial.print("connecting to mqtt broker...");
  #endif

  while ((!mqttClient.connect(_mqttId, _mqttUser, _mqttPwd)) && (millis() - connectStart < 5000)){
    #ifdef JBDOMODEBUG
    Serial.print(".");
    #endif
    delay(200);
  }

  if (!mqttClient.isConnected()) {
    #ifdef JBDOMODEBUG
    Serial.println("Failed!");
    #endif
    return -1;
  }

  #ifdef JBDOMODEBUG
  Serial.println("Success");
  #endif

  if (_rootTopic.length() > 0) {
    mqttClient.setWill(_lwtTopic, _offlineLwt);
    publish(_lwtTopic, _onlineLwt, true, 0);
  }

  _mqttClient = &mqttClient;

  
  return 1;
}

bool DomoMQTTClient::update() {
  return _mqttClient->update();
}

/** Publishing **/

success_t DomoMQTTClient::publishStatus(String status) {
  return publish(_statusTopic, status, true);
}

success_t DomoMQTTClient::publishCmd(String cmd) {
  return publish(_cmdTopic, cmd, false, (uint8_t)1U );
}

success_t DomoMQTTClient::publish(String topic, String payload, bool retain = false, uint8_t qos = (uint8_t)0U) {

}

/** Simple setters **/

void DomoMQTTClient::setRootTopic(String rootTopic) { _rootTopic = rootTopic; }

void DomoMQTTClient::setCmdTopic(String cmdTopic) { _cmdTopic = cmdTopic; }

void DomoMQTTClient::setstatusTopic(String statusTopic) { _statusTopic = statusTopic; }

void DomoMQTTClient::setLwtTopic(String lwtTopic) { _lwtTopic = lwtTopic; }

void DomoMQTTClient::setOnCmd(String onCmd) { _onCmd = onCmd; }

void DomoMQTTClient::setOffCmd(String offCmd) { _offCmd = offCmd; }

void DomoMQTTClient::setOnlineLwt(String onlineLwt) { _onlineLwt = onlineLwt; }

void DomoMQTTClient::setOfflineLwt(String offlineLwt) { _offlineLwt = offlineLwt; }