#include <RCSwitch.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define ENABLE_READ_VCC
long LIGHTS_OUT = 1394006;
long LIGHTS_ON = 1394007;

/***************** MQTT Setup **********/
#define WIFI_AP "Hotspot"
#define WIFI_PASSWORD "wifipwd1"
const char* mqtt_server = "192.168.43.108";

WiFiClient espClient;
PubSubClient client(espClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;
/****************** Button stuff ********/
RCSwitch mySwitch = RCSwitch();
int buttonState = 0;
int lightState = 0;
int buttonPin = 10;


void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  //TODO: Flash led fast while init
  InitWiFi();
  client.setServer(mqtt_server, 1883);
  lastSend = 0;
  mySwitch.enableTransmit(2);
  pinMode(buttonPin, INPUT_PULLUP);
  //TODO: stop flash
  if ( !client.connected() ) {
    reconnect();
  }
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

}

void toggle() {
  if (lightState == 1) {
    mySwitch.send(LIGHTS_ON, 24);
    lightState = 0;
  } else {
    mySwitch.send(LIGHTS_OUT, 24);
    lightState = 1;
  }
}
void loop() {
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendData();
    lastSend = millis();
  }

  buttonState = digitalRead(buttonPin);
  Serial.print(buttonState);
  if (buttonState == 0) {
    toggle();
    publishClick();
    delay(500);
  }

  client.loop();
}

void publishClick() {

  String payload = "{";
  payload += "\"lastclick\":"; payload += millis();
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/button/lastclick", attributes );
  Serial.println( attributes );
}
void getAndSendData()
{
  // Prepare a JSON payload string
  int val = analogRead(A0);

  String payload = "{";
  payload += "\"battery\":"; payload += ((val / 1024) * 100);
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/button/battery", attributes );
  Serial.println( attributes );
}

void InitWiFi()
{
  // initialize serial for ESP module
  // initialize ESP module
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);

  //If keeps failing, don't bother waiting forever. The system needs to work offline too
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 11) {
    delay(500);
    Serial.print(".");
    counter++;
  }
}

void reconnect() {
  // Loop until we're reconnected
  int counter = 0;
  while (!client.connected() && counter < 11) {
    Serial.print("Connecting to ThingsBoard node ...");
    if ( client.connect("Arduino Uno Device", "w", NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
    counter++;
  }
}
