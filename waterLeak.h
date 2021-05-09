#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "YOURssid";
const char* password = "YOURpassword";
const char* mqtt_server = "YOURip address";
const char* MQTTclientId = "YOURleakDetector1";
const char* MQTTuserName = "YOURuser";
const char* MQTTpassWord = "YOURpassword";
#define sensor    A0       // Hook water sensor to pin A0 of NODEMCU module
#define LED       D4       // Led in NodeMCU/WeMos D1 Mini at pin GPIO2 (D4)
#define BRIGHT    350      // Max led intensity (1-500)
#define INHALE    2500     // Inhalation time in milliseconds.
#define PULSE     INHALE*1000/BRIGHT
#define REST      10000    // Rest Between Inhalations.

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// void setup() {

// }

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(MQTTclientId,MQTTuserName,MQTTpassWord)) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish("waterDetector/rightDishwasher", "MQTT connected");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
    // put your setup code here, to run once:
  pinMode(sensor, INPUT);   // Analog pin setup for read
  pinMode(LED, OUTPUT);     // LED pin as output.
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    // A fancy delay routine! Makes the LED appear to "breathe" by slowly ramping up and down. This is to show you the water sensor is working 
  // by slowly making the LED on the board glow from dim to bright and back dim. It's not really needed but it's a bit of fun to watch.
  // If you don't add some delay the water sensor will attempt to "log" data often and your free Ubidots account may be disabled. This 
  // delay is intended to create a new data point once every 15 seconds or so. 
  
  for (int i = 1; i < BRIGHT; i++) {
    digitalWrite(LED, LOW);            // turn the LED on.
    delayMicroseconds(i * 10);         // wait
    digitalWrite(LED, HIGH);           // turn the LED off.
    delayMicroseconds(PULSE - i * 10); // wait
    delay(0);                          // to prevent watchdog firing.
  }
  //ramp decreasing intensity, Exhalation (half time):
  for (int i = BRIGHT - 1; i > 0; i--) {
    digitalWrite(LED, LOW);            // turn the LED on.
    delayMicroseconds(i * 10);         // wait
    digitalWrite(LED, HIGH);           // turn the LED off.
    delayMicroseconds(PULSE - i * 10); // wait
    i--;
    delay(0);                          // to prevent watchdog firing.
  }
  delay(REST);                         // take a rest...


  // Publish values of ADC0 water sensor. Water will cause the voltage to rise and the ADC will read this as a higher value.
  // Once the value is read the NODEMCU will publish it to UBIDOTS. The Node MCU does not care what the reading is. It only reports it.
  // If below trigger value the text message will NOT be delivered. Above trigger it's sent.

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    float adcValue = analogRead(sensor);    // Read the ADC channel
    // client.add("h2o_heater", adcValue);     // Variable for the water heater sensor assigned the ADC value. This will show up in Ubidots within the water-sensor device
    // client.publish("water-sensor");
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", adcValue);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("waterDetector/rightDishwasher", msg);
  }
}