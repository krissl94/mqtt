# Smart bike light!

For this project, you'll need the following hardware:
 
 * A raspberry pi 
 * 2 arduino's with wireless capabilities. We've used 2 NodeMCU ESP8266's
 * A 433MHz transmitter and receiver
 * A basic button, some resitors and leds for the lights 
 
The hardware setup can be seen in the documentation and the fritzing diagrams.
 
 
In order to run this project, be sure to start the MQTT broker first. 

> nodejs app.js

Then, upload the scrips to the arduinos. You'll have to set your MQTT broker's IP address and your wifi's SSID and WPA2 key in the sketch before you upload them. 
