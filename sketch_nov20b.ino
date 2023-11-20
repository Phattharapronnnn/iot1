#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "SAMSUNGPHAT";
const char* password = "1234567890";
const char* mqtt_server = "192.168.4.253";

WiFiClient espClient;
PubSubClient client(espClient);

float Temp;
float Hum;
DHT dht(D4, DHT11);

const int ledPin = 12; // Pin for the LED

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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("led_control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); // Set the callback function for handling MQTT messages
  client.subscribe("led_control"); // Subscribe to the "led_control" topic
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  Temp = dht.readTemperature();
  Hum = dht.readHumidity();

  // Check if the sensor read was successful
  if (isnan(Temp) || isnan(Hum)) {
    Serial.println("Failed to read from DHT sensor!");
    client.publish("dht11", "Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(Temp);
  Serial.print("Humidity: ");
  Serial.println(Hum);

  // client.publish("dht11", ("Temperature " + String(Temp) + " Celsius").c_str());
  // client.publish("dht11", ("Humidity " + String(Hum) + " Percent").c_str());
  client.publish("dht11", (String(Hum) + "+" + String(Temp)).c_str());
  delay(2000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received:");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is "ON" and turn on the LED
  if (message == "OFF") {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}