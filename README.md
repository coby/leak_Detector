# leak_Detector
Local, smart water leak detector using NodeMCU, Moisture Sensor, and MQTT. Works with Home Assistant and Node Red.

Use a NodeMCU and a "funduino" or other type of water sensor to create a local, plug-in leak detector.

You can utilize these instructions (https://www.instructables.com/DIY-ESP8266-WiFi-Water-Sensor-With-Text-and-Email-/) to set up the hardware, however that example uses a cloud-based MQTT server.

Use this code in the Arduino IDE (https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f) to Sketch > Export compiled Binary (after you have entered your information), then upload to NodeMCU with Tasmotizer or similar software (I had to utilize a jig and wasn't able to use USB).
