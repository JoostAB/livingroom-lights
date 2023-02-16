#include <config.h>

MyConfig::MyConfig() {}

void MyConfig::init() {}

void MyConfig::setMqttPort(uint16_t port) {
  _mqttPort = port;
}

uint16_t MyConfig::getMqttPort() {
  return _mqttPort;
}

void MyConfig::setMqttServer(const char* server) {
  PRINTLN("YES!!!!! We are setting ", server)
  _saveStrCopy(_mqttServer, server, sizeof(_mqttServer));
}

char* MyConfig::getMqttServer() {
  return _mqttServer;
}

void MyConfig::setMqttUser(const char* user) {
  _saveStrCopy(_mqttUser, user, sizeof(_mqttUser));
}

char* MyConfig::getMqttUser() {
  return _mqttUser;
}

void MyConfig::setMqttPassword(const char* passw) {
  _saveStrCopy(_mqttPassword, passw, sizeof(_mqttPassword));
}

char* MyConfig::getMqttPassword() {
  return _mqttPassword;
}

void MyConfig::setMqttTopic(const char* topic) {
  _saveStrCopy(_mqttTopic, topic, sizeof(_mqttTopic));
}

char* MyConfig::getMqttTopic() {
  return _mqttTopic;
}

bool MyConfig::_saveStrCopy(char* tgt, const char* src, size_t maxlen) {
  if (strlen(src) > maxlen) {
    PRINTF2("Max length of %u exceeded: %u", strlen(src), maxlen)
    return false;
  }
  strcpy(tgt, src);
  return true;
}

