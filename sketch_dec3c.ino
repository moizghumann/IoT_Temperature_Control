#include <WiFi.h>

#include <ArduinoMqttClient.h>

#include<DHT.h>


const char* wifiSsid = "kebab404";
const char* wifiPassword = "pingpong818";

// mqtt broker
const char* mqttUrl = "mqtt.api.grandeur.tech";
int port = 1883;

const char* mqttUsername = "grandeurlpokjnaw1grk0jhm178v4l07"; // API key
const char* mqttPassword = "28204003a68e2fb4954e8e58ed660ad93434edd4ac318ba6450e6abe897fd082"; // Device token

String deviceID = "devicelpokk49s1gro0jhmh1cmgza5";

#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LEDPIN 4

float prev = 0.0;
float temp = 0.0;
int toggleLed = 0;
String toggleMessage;


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(LEDPIN, OUTPUT);

  connectWiFi();
  connectMQTT();
  
  // subscribing to ("deviceljel151y45z60jjhhz26detv/temp") topic
  mqttClient.subscribe(deviceID + "/temp");
  mqttClient.subscribe(deviceID + "/toggleLed");

  // recieve message callback function
  mqttClient.onMessage(onMqttMessage);
}

void loop() {
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keeps alive which avoids being disconnected by the broker
  mqttClient.poll();

  temp = dht.readTemperature();
  Serial.println(temp);
  
  // topic set up
  mqttClient.beginMessage(deviceID + "/temp");  // topic -> (deviceID + "/temp")
  // publishes temp data to topic 
  mqttClient.print(temp);
  mqttClient.endMessage();
  
  delay(5000);
}

void connectWiFi() {
    // Connecting to a WiFi network

  Serial.println(); //printlnbreaks line
  Serial.println();
  Serial.print("Connecting to ");  //print doesnt break line
  Serial.println(wifiSsid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with other WiFi-devices on WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
}

void connectMQTT() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqttUrl);

  mqttClient.setUsernamePassword(mqttUsername, mqttPassword);

  if (!mqttClient.connect(mqttUrl, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(100);
    while (1);
  }
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}


void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  if (mqttClient.messageTopic() == (deviceID + "/temp")) {

    String newTemp = "";

    while (mqttClient.available()) {
      newTemp += (char)mqttClient.read(); // 2 + 2 = 22
    } 

    temp = newTemp.toFloat();
    Serial.println();
    Serial.println(temp);
    if (prev == temp) {
      return;
    }  
  

    prev = temp; // prev = 40

    if (temp > 22.0) {
    
      mqttClient.beginMessage(deviceID + "/toggleLed");
      mqttClient.print(1);
      mqttClient.endMessage();
    } else {
      mqttClient.beginMessage(deviceID + "/toggleLed");
      mqttClient.print(0);
      mqttClient.endMessage();
    }
  }


  if (mqttClient.messageTopic() == (deviceID + "/toggleLed")) {

    toggleMessage = "";
     
    while (mqttClient.available()) {
      toggleMessage = (char)mqttClient.read();
      Serial.print(toggleMessage);
    }
  
  
    if (toggleMessage.equals("1")) {
      digitalWrite(LEDPIN, HIGH);
    } else {
       digitalWrite(LEDPIN, LOW);
    }
  }
}

