#include <Arduino.h>
#include <jbkaku.h>
#include <WiFiManager.h>
#include <mqtt_client.h>
#include <jb_domo_mqtt.h>

DomoMQTTClient mqtt("192.168.0.10");

ESP8266WebServer server(80);

WiFiManager wifiManager;

void handleNotFound() {
    server.send(404, "text/plain", "404: File Not Found");
}

void handleCommand() {
    String cmd = server.uri();
    mqtt.setOffCmd("asd");
    #ifdef DEBUGLOG
    Serial.print("Command received: ");
    Serial.println(cmd);
    #endif
    
    if (cmd.equals("/switchlighton")) {
        setLightsOn();
        server.send(200, "text/plain", "Lights switched on");
    } else if (cmd.equals("/switchlightoff")) {
        setLightsOff();
        server.send(200, "text/plain", "Lights switched off");
    } else {
      handleNotFound();
    }
    // if (cmd.equals("/setup")) {
    //     wifiManager.startConfigPortal();
    // }
    
}

void startWebServer() { 
    // server.on("/switchlighton",  HTTP_GET, []() {
    //         server.send(200, "text/plain", "Get received.");
    //     }, handleCommand);
    // server.on("/switchlightoff",  HTTP_GET, []() {
    //         server.send(200, "text/plain", "Get received.");
    //     }, handleCommand);
    // server.on("/setup",  HTTP_POST, []() {
    //         server.send(200, "text/plain", "Post received.");
    //     }, handleCommand);
    server.onNotFound(handleCommand);

    // start the HTTP server
    server.begin();
}

void setup() {
    #ifdef DEBUGLOG
    Serial.begin(SERIALBAUDRATE);
    #endif
    

    // Setup WiFiManager. 
    // Uncomment following line to test WiFimanager. This will delete any stored AP's
    // WiFi.disconnect(); 
    #ifdef DEBUGLOG
    Serial.println("Starting wifi...");
    #endif
    
    //WiFiManager wifiManager;
    
    #ifndef DEBUGLOG
    wifiManager.setDebugOutput(false);
    #endif
    wifiManager.autoConnect("JoJoRoBa light");

    #ifdef DEBUGLOG
    Serial.print("Connected to wifi. My address: ");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
    #endif
    
    startWebServer();

    mqtt_setup();

    #ifdef DEBUGLOG
    Serial.println("All ok...");
    #endif
}

void loop() {
    server.handleClient();
    mqtt.update();
    mqtt_loop();
}


