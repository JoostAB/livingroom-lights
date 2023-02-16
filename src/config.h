# pragma once
#ifndef __MY_CONFIG_H__
#define __MY_CONFIG_H__

#include <Arduino.h>
#include <jbdebug.h>

class MyConfig {
  public:
    MyConfig();    
    
    void 
      init(),
      setMqttPort(uint16_t port),
      setMqttServer(const char* server),
      setMqttUser(const char* user),
      setMqttPassword(const char* passw),
      setMqttTopic(const char* topic);


    /**
     * @brief Returns port of the mqtt server, as registered in config portal
     * 
     * @return uint16_t 
     */
    uint16_t getMqttPort();

    /**
     * @brief Returns hostname of the mqtt server, as registered in config portal
     * 
     * @return char* 
     */
    char* getMqttServer();

    /**
     * @brief Returns user of the mqtt server, as registered in config portal
     * 
     * @return char* 
     */
    char* getMqttUser();

    /**
     * @brief Returns user password of the mqtt server, as registered in config portal
     * 
     * @return char* 
     */
    char* getMqttPassword();

    /**
     * @brief Returns main mqtt topic, as registered in config portal. If the topic ends with 
     * a forward slash, it is stripped
     * 
     * @return char* 
     */
    char* getMqttTopic();

    

  private:
    uint16_t _mqttPort = 1883;
    
    bool _saveStrCopy(char* tgt, const char* src, size_t maxlen);
    char 
      _mqttServer[40],
      _mqttUser[40],
      _mqttPassword[40],
      _mqttTopic[40] = "homelight/livingroom/main/";
      
};

#endif
