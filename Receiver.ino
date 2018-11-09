#define _DEBUG_
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ThingerESP8266.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER

#define USERNAME "krissl94"
#define DEVICE_ID "light1"
#define DEVICE_CREDENTIAL "light1"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
boolean lightState = false;
#define WIFI_AP "WIFI"
#define WIFI_PASSWORD "WPA"
const char* mqtt_server = "192.168.178.185";

#include <RCSwitch.h>

WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  Serial.print("booting");
  mySwitch.enableReceive(2);  // Receiver on interrupt 0 => that is pin #2
  //  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  thing.add_wifi(WIFI_AP, WIFI_PASSWORD);
  thing["toggleLight"] << [](pson & in) {
    toggleLight();
  };
  thing["lightState"] >>  [](pson & out) {
    out = lightState;
  };
  pinMode(5, OUTPUT);
  reconnect();
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_AP);

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    setLight(1);
  } else {
    setLight(0);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("v1/devices/light/connection", "Connected..!");
      client.subscribe("v1/devices/light/state");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void toggleLight() {
  setLight(!lightState);
}

void setLight(boolean lightOn) {
  if (lightOn) {
    digitalWrite(5, 1);
    lightState = true;
  } else {
    digitalWrite(5, 0);
    lightState = false;
  }

  String payload = "{";
  payload += "\"state\":"; payload += lightState;
  payload += "}";
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish("v1/devices/light/state/", attributes);
}
void loop() {
  if (mySwitch.available()) {
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    if (mySwitch.getReceivedValue() == 1394007) {
      setLight(true);
    } else {
      setLight(false);
    }
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
  }
  thing.handle();
  client.loop();
}
