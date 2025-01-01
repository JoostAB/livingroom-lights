/**
 * @file webui.h
 * 
 * @author Joost Bloemsma (https://github.com/JoostAB)
 * @version 0.0.1
 * @date 2023-12-15
 */

# pragma once
#ifndef __WEB_UI_H__
#define __WEB_UI_H__

#include <general.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>

bool webServerRunning = false;

ESP8266WebServer webServer(80);

void handleFileRequest();
bool handleFileRead(String path);

void webui_start() {
  webServer.onNotFound(handleFileRequest);
  webServer.begin();
  webServerRunning = true;
}

void webui_loop() {
  if (webServerRunning) {
    webServer.handleClient();
  }
}

void handleFileRequest() {
  if(!handleFileRead(server.uri())){
      // if the requested file or page doesn't exist, return a 404 not found error
      server.send(404, "text/plain", "404: File Not Found");
  }
}

#endif // __WEB_UI_H__