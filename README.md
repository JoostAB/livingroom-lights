# Livingroom lights

Switches the lights in the livingroom using KlikAanKlikUit switches.
KaKu switching is done using the NewRemoteSwitch library by Randy Simons http://randysimons.nl/
and was usually found here: https://bitbucket.org/fuzzillogic/433mhzforarduino but now I got
it from https://github.com/hjgode/homewatch/tree/master/arduino/libraries/NewRemoteSwitch with
some small adjustments (removed dependency on PinChangeInt)

But since this library is nowhere to be found anymore, some alternatives are listed below:


> **_Note:_** The NewRemoteSwitch library can be downloaded from https://github.com/1technophile/NewRemoteSwitch. This is also a dependency in platformio_envs.ini.

> **_Warning:_** This version only works with ESP8266 devices. For some reason I can't get the NewRemoteSwitch library to work correctly on ESP32

## Enhancements since initial version:

- Listens to actions from actual remotes
- Listens to MQTT commands
- Publishes actual state (from external remote or self) to MQTT

## Todo:

- [X] Implement a wifi manager
- [ ] ESP32 compatibility
- [ ] OTA (startable using MQTT command)
- [ ] Autodiscovery in HomeAssistant
- [ ] Training option for KaKu addresses
- [ ] Dimming
- [X] Use LED_BUILTIN for status info (connecting etc)
- [ ] Cleanup code (ongoing project ;))