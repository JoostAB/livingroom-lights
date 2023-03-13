# pragma once
#ifndef __MY_CONFIG_H__
#define __MY_CONFIG_H__

#include <Arduino.h>
#include <jbdebug.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class MyConfig {
  public:
    MyConfig();    
    
    void 
      init(),
      setMqttPort(const char* port),
      setMqttServer(const char* server),
      setMqttUser(const char* user),
      setMqttPassword(const char* passw),
      setMqttTopic(const char* topic),
      setCfgFileName(const char* fname),
      getMqttCfg(char* port, char* server, char* user, char* password, char* topic);
    
    bool
      saveConfig(),
      readConfig();


    /**
     * @brief Returns port of the mqtt server, as registered in config portal
     * 
     * @return String 
     */
    String getMqttPort();

    /**
     * @brief Returns hostname of the mqtt server, as registered in config portal
     * 
     * @return String
     */
    String getMqttServer();

    /**
     * @brief Returns user of the mqtt server, as registered in config portal
     * 
     * @return String
     */
    String getMqttUser();

    /**
     * @brief Returns user password of the mqtt server, as registered in config portal
     * 
     * @return String 
     */
    String getMqttPassword();

    /**
     * @brief Returns main mqtt topic, as registered in config portal. If the topic ends with 
     * a forward slash, it is stripped
     * 
     * @return String 
     */
    String getMqttTopic();

    String getCfgFileName();
    

    

  private:
    
    bool 
      _saveStrCopy(char* tgt, const char* src, size_t maxlen),
      _fsBegin();

    char 
      _mqttPort[6] = "1883",
      _mqttServer[40],
      _mqttUser[40],
      _mqttPassword[40],
      _mqttTopic[40] = "homelight/livingroom/main/";
    
    String _cfgFilename = "/wificfg.json";

    bool _fsBegun = false;
      
};

#endif
