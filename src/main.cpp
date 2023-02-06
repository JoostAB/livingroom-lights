#include <Arduino.h>

#define DEBUGLOG 1

#include <jbdebug.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <jbkaku.h>
#include <jb_mqtt_client.h>

// #include <WiFiClient.h>
// #include <jb_domo_mqtt.h>

const char* ssid = "jojodomo";
const char* password = "jojoDomo";

void mqttCommandReceived(const String& cmd) {
  if (cmd == "ON") {
    setLightsOn();
  } else {
    setLightsOff();
  }
}

void kakuCodeReceived(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
  
}

void setup() {
  #ifdef DEBUGLOG
  Serial.begin(115200);
  #endif
  
  // Setup WiFiManager. 
  // Uncomment following line to test WiFimanager. This will delete any stored AP's
  // WiFi.disconnect(); 
  PRINTS("Starting wifi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    PRINTS(".");
  }

  PRINTLF;
  PRINTLN("Connected to ", ssid);
  PRINTLN("IP address: ", WiFi.localIP());
  startReceiver(kakuCodeReceived);
  mqtt_setup(mqttCommandReceived);
}

void loop() {
  mqtt_loop();
}



