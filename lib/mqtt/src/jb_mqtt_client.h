#include <Arduino.h>
#include <jbdebug.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
//#include <jbkaku.h>
#include <MQTTPubSubClient.h>

typedef std::function<void(const String& cmd)> MsgReceivedCB;

MsgReceivedCB _cmdCallback;

WiFiClient wifiClient;
MQTTPubSubClient mqttClient;

struct mqtt_client_info
{
  const char* mqttHost;
  uint16_t mqttPort;
  const char* mainTopic;
  const char* cmdTopic;
  
};

void handleCmd(const String& payload, const size_t size) {
  PRINTS("Message arrived [");
  PRINTDS(payload);
  PRINTLNS("]");

  _cmdCallback(payload);
}

void mqtt_setup(MsgReceivedCB callback) {
  Serial.print("connecting to host...");
  while (!wifiClient.connect("192.168.0.10", 1883)) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" connected!");


  mqttClient.begin(wifiClient);
  Serial.print("connecting to mqtt broker...");
  while (!mqttClient.connect("homelight", "mqtt", "mqtt_user")) {
    PRINTS(".");
    delay(200);
  }
  PRINTLNS(" connected!");
  mqttClient.setWill("homelight/livingroom/main/lwt","offline", true, 0);
  mqttClient.subscribe("homelight/livingroom/main/cmd", handleCmd);
  mqttClient.publish("homelight/livingroom/main/ip", wifiClient.localIP().toString(), true, 0);
  mqttClient.publish("homelight/livingroom/main/lwt", "online", true, 0);

  _cmdCallback = callback;
}

void mqtt_loop() {
  mqttClient.update();
}

void mqtt_setstatus(const String status) {
  mqttClient.publish("homelight/livingroom/main/status", status, true, 0);
}

void mqtt_pub(const String& topic, const String& payload) {
  mqttClient.publish(topic, payload, true, 0);
}

void mqtt_pub_status() {

}