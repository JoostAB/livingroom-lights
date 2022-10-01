#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <jbkaku.h>
#include <MQTTPubSubClient.h>

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
 #ifdef DEBUGLOG
  Serial.print("Message arrived [");
  Serial.print(payload);
  Serial.println("]");
#endif

  if (payload == "ON") {
    setLightsOn();
  } else {
    setLightsOff();
  }
}

void mqtt_setup() {
  Serial.print("connecting to host...");
  while (!wifiClient.connect("192.168.0.10", 1883)) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" connected!");


  mqttClient.begin(wifiClient);
  Serial.print("connecting to mqtt broker...");
  while (!mqttClient.connect("homelight", "mqtt", "mqtt_user")) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" connected!");
  mqttClient.setWill("homelight/livingroom/main/lwt","offline", true, 0);
  mqttClient.subscribe("homelight/livingroom/main/cmd", handleCmd);
  mqttClient.publish("homelight/livingroom/main/ip", wifiClient.localIP().toString(), true, 0);
}

void mqtt_loop() {
  mqttClient.update();
}

void mqtt_setstatus(const String status) {
  mqttClient.publish("homelight/livingroom/main/status", status, true, 0);
}
