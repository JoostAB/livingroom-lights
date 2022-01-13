#include <Arduino.h>
#include <jbkaku.h>
#include <WiFiManager.h>

#define JBDEBUG

#define TRANSMITPIN D1

#define KAKU_ADDRESS KAKU_ADDRESS_WALL

// Create a transmitter on address KAKU_ADDRESS, using digital pin TRANSMITPIN to transmit, 
// with a period duration of 260ms (default), repeating the transmitted
// code 2^3=8 times.
NewRemoteTransmitter transmitter(KAKU_ADDRESS, TRANSMITPIN, 264, 4);
ESP8266WebServer server(80);

void setLightsOn() {
    #ifdef JBDEBUG
    Serial.println("Switching all on...");
    #endif
    
    transmitter.sendUnit(WALL_UNIT, true);
    server.send(200, "text/plain", "Lights switched on");
}

void setLightsOff() {
    #ifdef JBDEBUG
    Serial.println("Switching all off..");
    #endif
    
    transmitter.sendUnit(WALL_UNIT, false);
    server.send(200, "text/plain", "Lights switched off");
}

void handleNotFound() {
    server.send(404, "text/plain", "404: File Not Found");
}

void handleCommand() {
    String cmd = server.uri();

    #ifdef JBDEBUG
    Serial.print("Command received: ");
    Serial.println(cmd);
    #endif
    
    if (cmd.equals("/switchlighton")) {
        setLightsOn();
    }
    if (cmd.equals("/switchlightoff")) {
        setLightsOff();
    }
}

void startWebServer() { 
    server.on("/switchlighton",  HTTP_POST, []() {
            server.send(200, "text/plain", "Post received.");
        }, handleCommand);
    server.on("/switchlightoff",  HTTP_POST, []() {
            server.send(200, "text/plain", "Post received.");
        }, handleCommand);
    server.onNotFound(handleCommand);

    // start the HTTP server
    server.begin();
}

void setup() {
    #ifdef JBDEBUG
    Serial.begin(115200);
    #endif
    

    // Setup WiFiManager. 
    // Uncomment following line to test WiFimanager. This will delete any stored AP's
    // WiFi.disconnect(); 
    #ifdef JBDEBUG
    Serial.println("Starting wifi...");
    #endif
    
    WiFiManager wifiManager;
    
    #ifndef JBDEBUG
    wifiManager.setDebugOutput(false);
    #endif
    
    wifiManager.autoConnect("JoJoRoBa light");

    #ifdef JBDEBUG
    Serial.print("Connected to wifi. My address: ");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
    #endif
    
    startWebServer();

    #ifdef JBDEBUG
    Serial.println("All ok...");
    #endif
}

void loop() {
    server.handleClient();
}


