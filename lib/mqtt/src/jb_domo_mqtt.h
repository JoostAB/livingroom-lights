/**
 * @file jb_domo_mqtt.h
 * @author Joost Bloemsma (joost.a@bloemsma.net)
 * @brief 
 * @version 1.0
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _Jb_Domo_Mqtt_
#define _Jb_Domo_Mqtt_

#include <Arduino.h>
#include <MQTTPubSubClient.h>

#define JBDOMODEBUG
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  typedef WiFiClient WFClient;
#endif

typedef int8_t success_t;

class DomoMQTTClient {
  public: 
    DomoMQTTClient(String host, uint16_t port = 1883);
    DomoMQTTClient(String host, String rootTopic, uint16_t port = 1883);
    DomoMQTTClient(String host, String rootTopic, String cmdTopic = "cmd", String statusTopic = "status", 
        String lwtTopic = "lwt", String onCmd = "ON", String offCmd = "OFF", String onlineLwt = "Online",
        String offlineLwt = "Offline", uint16_t port = 1883);

    void
      setRootTopic(String rootTopic),
      setCmdTopic(String cmdTopic),
      setstatusTopic(String statusTopic),
      setLwtTopic(String lwtTopic),
      setOnCmd(String onCmd),
      setOffCmd(String offCmd),
      setOnlineLwt(String onlineLwt),
      setOfflineLwt(String offlineLwt);
    
    bool update();
    
    success_t
      start(),
      start(WFClient* wifiClient),
      publishStatus(String status),
      publishCmd(String cmd),
      publish(String topic, String payload, bool retain = false, uint8_t qos = (uint8_t)0U);

  private:
    uint16_t _port = 1883;
    String 
          _host,
          _rootTopic,
          _cmdTopic,
          _statusTopic,
          _lwtTopic,
          _onCmd,
          _offCmd,
          _onlineLwt,
          _offlineLwt,
          _mqttId,
          _mqttUser,
          _mqttPwd;
    WFClient* _wifiClient;
    MQTTPubSubClient* _mqttClient;
};

#endif