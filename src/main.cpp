#include <Arduino.h>
#define DEBUGLOG 1

#include <jbdebug.h>
#include <mykaku.h>
#include <mymqtt.h>

const char* cmdOn = "ON";
const char* cmdOff = "OFF";

void kakuReceived(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
  PRINTLN("KAKU command received from ", sender)
  if (switchType == 1) {
    mqtt_setStatus(cmdOn);
  } else {
    mqtt_setStatus(cmdOff);
  }
}

void MQTTCmdReceived(const char* cmd) {
  PRINTLN("MQTT command received: ", cmd);
  int sc = strcmp(cmdOn, cmd);
  PRINTLN("Stringcompare returned: ", sc);
  if (strcmp(cmdOn, cmd) == 0) {
    kaku_setLightsOn();
    mqtt_setStatus(cmdOn);
  } else {
    kaku_setLightsOff();
    mqtt_setStatus(cmdOff);
  }
}

void setup() {
  DEBUGSTARTDEF
  kaku_start(kakuReceived);
  mqtt_start(MQTTCmdReceived);
}

void loop() {
  mqtt_loop();
}



// #ifdef ESP8266
// #include <ESP8266WiFi.h>
// #elif defined(ESP32)
// #include <WiFi.h>
// #endif

// // #include <jbkaku.h>
// // #include <jb_mqtt_client.h>

// // #include <WiFiClient.h>
// // #include <jb_domo_mqtt.h>

// const char* ssid = "jojodomo";
// const char* password = "jojoDomo";
// // #include <NewRemoteReceiver.h> 

// // void showCode(unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
// //   Serial.print("Code (address): ");
// //   Serial.println(address);
// //   Serial.print(" Period: ");
// //   Serial.println(period);
// //   Serial.print(" unit: ");
// //   Serial.println(unit);
// //   Serial.print(" groupBit: ");
// //   Serial.println(groupBit);
// //   Serial.print(" switchType: ");
// //   Serial.println(switchType);

// //   if (dimLevelPresent){
// //     Serial.print(" dimLevel: ");
// //     Serial.println(dimLevel);    
// //   }
// // }

// // void setup() {
// //   Serial.begin(115200);
// //   NewRemoteReceiver::init(5, 2, showCode);
// //   Serial.println("Receiver initialized");    
// // }
// // void loop() {}


// #define DEBUGLOG 1

// #include <jbdebug.h>

// // #ifdef ESP8266
// // #include <ESP8266WiFi.h>
// // #elif defined(ESP32)
// // #include <WiFi.h>
// // #endif

// #include <jbkaku.h>
// #include <jb_mqtt_client.h>

// // #include <WiFiClient.h>
// // #include <jb_domo_mqtt.h>

// // const char* ssid = "jojodomo";
// // const char* password = "jojoDomo";

// void mqttCommandReceived(const String& cmd) {
//   PRINTLN("MQTT command received: ", cmd);
//   if (cmd == "ON") {
//     setLightsOn();
//     mqtt_pub_status("ON");
//   } else if (cmd == "OFF") {
//     setLightsOff();
//     mqtt_pub_status("OFF");
//   } else if (cmd == "REBOOT") {
//     PRINTLNS("Rebooting,,,");
//     delay(1000);
//     ESP.restart();
//   } else {
//     PRINTLNS("Unknown command received");
//   }
// }

// void kakuCodeReceived(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType) {
//   PRINTLNS("KAKU command received");
// }

// // void setup() {
// //   #ifdef DEBUGLOG
// //   Serial.begin(115200);
// //   #endif
// //   Serial.println("Starting device");
// //   startReceiver(kakuCodeReceived);
// // }

// void setup() {
//   DEBUGSTARTDEF
  
//   // Setup WiFiManager. 
//   // Uncomment following line to test WiFimanager. This will delete any stored AP's
//   // WiFi.disconnect();
//   PRINTLF
//   PRINTS("Starting wifi...")
  
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
  
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     PRINTS(".")
//   }

//   PRINTLF;
//   PRINTLN("Connected to ", ssid);
//   PRINTLN("IP address: ", WiFi.localIP());
//   mqtt_setup(mqttCommandReceived);
  
//   startReceiver(kakuCodeReceived);

// }

// void loop() {
//   mqtt_loop();
// }



