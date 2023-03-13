#include <config.h>

MyConfig::MyConfig() {}

void MyConfig::init() {}

bool MyConfig::saveConfig() {
  PRINTLNS("Writing configuration to filesystem.")

  if (!_fsBegin()) return false;

  DynamicJsonDocument json(1024);
      
  json["mqtt_server"] = _mqttServer;
  json["mqtt_port"] = _mqttPort;
  json["mqtt_user"] = _mqttUser;
  json["mqtt_password"] = _mqttPassword;
  json["mqtt_topic"] = _mqttTopic;

  File cfgFile = LittleFS.open(_cfgFilename,"w");

  if (!cfgFile) {
    PRINTLN("Error opening file ", _cfgFilename)
    return false;
  }

  size_t size = serializeJson(json, cfgFile);
  cfgFile.close();

  if (size > 0) {
    PRINTF2("Written %u bytes to %s.", size, _cfgFilename.c_str())
    PRINTLF
    return true;
  }

  PRINTLN("Nothing written to ", _cfgFilename)
  return false;
}

bool MyConfig::readConfig() {
  PRINTLNS("Reading configuration from filesystem.")

  if (!_fsBegin()) return false;

  File cfgFile = LittleFS.open(_cfgFilename, "r");
  if (cfgFile) {
    PRINTLN("Reading config file: ", _cfgFilename);
    size_t size = cfgFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    cfgFile.readBytes(buf.get(), size);
    
    PRINTLNS("Parsing file to JSON")
    DynamicJsonDocument json(1024);
    auto deserializeError = deserializeJson(json, buf.get());
    
    if (!deserializeError ) {  
      PRINTLNS("File read succesfull.")
      IFDEBUG(
        String _tmpJson;
        serializeJsonPretty(json, _tmpJson);
        PRINTLNSA(_tmpJson)
      )
      strcpy(_mqttServer, json["mqtt_server"]);
      strcpy(_mqttPort, json["mqtt_port"]);
      strcpy(_mqttUser, json["mqtt_user"]);
      strcpy(_mqttPassword, json["mqtt_password"]);
      strcpy(_mqttTopic, json["mqtt_topic"]);
    } else {
      PRINTLN("Failed to parse ", _cfgFilename)
      return false;
    }
    cfgFile.close();
  } else {
    PRINTLN("Failed to open ", _cfgFilename)
    return false;
  }
  return true;
}

void MyConfig::setCfgFileName(const char* fname) {
  //_saveStrCopy(_cfgFilename, fname, 40);
  _cfgFilename = String(fname);
}

String MyConfig::getCfgFileName() {
  return String(_cfgFilename);
}

void MyConfig::setMqttPort(const char* port) {
  _saveStrCopy(_mqttPort, port, sizeof(_mqttPort));
}

String MyConfig::getMqttPort() {
  return String(_mqttPort);
}

void MyConfig::setMqttServer(const char* server) {
  _saveStrCopy(_mqttServer, server, sizeof(_mqttServer));
}

String MyConfig::getMqttServer() {
  return String(_mqttServer);
}

void MyConfig::setMqttUser(const char* user) {
  _saveStrCopy(_mqttUser, user, sizeof(_mqttUser));
}

String MyConfig::getMqttUser() {
  return String(_mqttUser);
}

void MyConfig::setMqttPassword(const char* passw) {
  _saveStrCopy(_mqttPassword, passw, sizeof(_mqttPassword));
}

String MyConfig::getMqttPassword() {
  return String(_mqttPassword);
}

void MyConfig::setMqttTopic(const char* topic) {
  _saveStrCopy(_mqttTopic, topic, sizeof(_mqttTopic));
}

String MyConfig::getMqttTopic() {
  return String(_mqttTopic);
}

void MyConfig::getMqttCfg(char* port, char* server, char* user, char* password, char* topic) {
  _saveStrCopy(port, _mqttPort, 6);
  _saveStrCopy(server, _mqttServer, 40);
  _saveStrCopy(user, _mqttUser, 40);
  _saveStrCopy(password, _mqttPassword, 40);
  _saveStrCopy(topic, _mqttTopic, 40);
}

bool MyConfig::_saveStrCopy(char* tgt, const char* src, size_t maxlen) {
  if (strlen(src) > maxlen) {
    PRINTF2("Max length of %u exceeded: %u", strlen(src), maxlen)
    return false;
  }
  strcpy(tgt, src);
  return true;
}

bool MyConfig::_fsBegin() {
  if (_fsBegun) return true;
  PRINTLNS("Mounting filesystem for parameters")
  if (!LittleFS.begin()) {
    PRINTLNS("Failed to mount filesystem")
    return false;
  }
  PRINTLNS("Filesystem mounted")
  _fsBegun = true;
  return true;
}
