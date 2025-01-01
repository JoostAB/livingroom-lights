# Livingroom lights

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build](https://github.com/JoostAB/livingroom-lights/actions/workflows/build.yml/badge.svg)](https://github.com/JoostAB/livingroom-lights/actions/workflows/build.yml)

Switches the lights in the livingroom using KlikAanKlikUit switches.
KaKu switching is done using the NewRemoteSwitch library by [Randy Simons](http://randysimons.nl/)
and was originally found on his [Bitbucket repo](https://bitbucket.org/fuzzillogic/433mhzforarduino).

Now, I use the [Github repo of 1technophile](https://github.com/1technophile/NewRemoteSwitch). This is also a dependency in platformio_envs.ini.

~~> **_Warning:_** This version only works with ESP8266 devices. For some reason I can't get the NewRemoteSwitch library to work correctly on ESP32~~
(Seems fixed now)

## Enhancements since initial version

- Listens to actions from actual remotes
- Listens to MQTT commands
- Publishes actual state (from external remote or self) to MQTT

## Todo

- [X] Implement a wifi manager (v1.0.0)
- [X] MQTT config using WiFiManager (v1.0.0)
- [X] ArduinoOTA (startable using MQTT command) (v1.0.0)
- [X] Autodiscovery in HomeAssistant (v1.0.0)
- [X] Use LED_BUILTIN for status info (connecting etc) (v1.0.0)
- [X] ESP32 compatibility (v1.0.1)
- [ ] Cleanup code (ongoing project :wink: )
- [ ] Add unit tests

## NTH (Nice to haves, so maybe)

- [ ] Own web interface for config
- [ ] Training option for KaKu addresses
- [ ] Dimming
- [ ] [MQTT-Now](https://github.com/JoostAB/mqtt-now) integration
