#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Network information 
const char* ssid = ""; 
const char* password = "";
const char* mqtt_server = "";//ip

//Sensor config(pin2, dht11)
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const char* sensor_id = "1234567890";
float temp;
float humid;

WiFiClient espClient;
PubSubClient client(espClient);

// Char variables for holding mqtt messages
#define MSG_BUFFER_SIZE  (100)
char temp_msg[MSG_BUFFER_SIZE];
char hum_msg[MSG_BUFFER_SIZE];

// Connecting to the WIFI network
void setup_wifi() {
  delay(10);
  
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
    if (client.connect(clientId.c_str(), "mqtt", "")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
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
  Serial.begin(9600);
  
  setup_wifi();

  client.setServer(mqtt_server, 1883);

  dht.begin();
  
}

void loop() {
  // Connect to the mqtt client
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Get the current object temperature and hmidity of the sensor
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  
  // Create the temperature message that will be send using mqtt
  String temperature = String("sensor1,location=bedroom temperature="+String(temp));
  temperature.toCharArray(temp_msg, temperature.length());
  Serial.println(temp_msg);

  // Create the humidity message that will be send using mqtt
  String humidity = String("sensor1,location=bedroom humidity="+String(humid));
  humidity.toCharArray(hum_msg, humidity.length());
  Serial.println(hum_msg);

  // Send the messages to the sensors topic
  client.publish("sensors", temp_msg);
  client.publish("sensors", hum_msg);

  delay(5000); 
}
