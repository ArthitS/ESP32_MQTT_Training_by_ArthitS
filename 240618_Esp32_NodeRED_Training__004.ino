#include <WiFi.h>           //Library WiFi for esp32
#include <PubSubClient.h>
#include <ArduinoJson.h>

//--------------------------------------------Variable Declaration[CanEdit]--------------------------------------------

//Variable for Subscription Example 1 --> Topic: "PiaHouse/bedroom/temperature"
double temperature;

//Variable for Subscription Example 3--> Topic: "FRA503/MaxMinNumber"
char json[100];
double Max1;
double Min1;

//Variable for Publishing Example 1 --> Topic: "PiaHouse/On_Off_PushButton"
int buttonPin = 23;
int buttonState = 0;

//Variable for Publishing Example 2 --> Topic: "PiaHouse/PotentiometerValue"
int PotentiometerPin = 34;
String PotentiometerValue;




//--------------------------------------------Setup Wifi & MQTT server[CanEdit]--------------------------------------------

//wifi value
const char* ssid = "PIA_PC";
const char* password = "piapiapia";


//mqtt value
const char* mqtt_server = "broker.mqttdashboard.com";
//const char *mqtt_username = "mqttUser";
//const char *mqtt_password = "FRA503IoT";
//const int mqtt_port = 1883;


//--------------------------------------------function MQTT--------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //--------------------------------------------(2nd Subscription zone)_Check the topic and recieve the payload for do someting by depend on your coding.[CanEdit]--------------------------------------------

  //Subscription Example 1: Recieve the payload value in string and how to convert them to use in program.
  if (topic == "PiaHouse/bedroom/temperature"){
    temperature = messageTemp.toDouble(); //Add .toDouble() to convert string to double
    Serial.println(temperature);
  }

  //Subscription Example 2: Recieve the payload and bring them to check the logic.
  else if (topic == "PiaDevice/ledColour"){
    if (messageTemp == "red") {
          Serial.print("text :");
          Serial.println("led red");
        }
        else if (messageTemp == "green") {
          Serial.print("text :");
          Serial.println("led green");
        }
        else if (messageTemp == "blue") {
          Serial.print("text :");
          Serial.println("led blue");
      }
  }

  //Subscription Example 3: Receive the payload in JSON format and deserialize them to make each data.
  else if (topic == "FRA503/MaxMinNumber") { 
    Serial.println(messageTemp);

    StaticJsonDocument<200> doc;
    String json = messageTemp;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    Max1 = doc["Max"];
    Min1 = doc["Min"];

    Serial.println(Max1);        //Debug the value from ESP32
    Serial.println(Min1);        //Debug the value from ESP32
  }

}

//--------------------------------------------(1st Subscription zone)_Subscribe the topic--------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    String client_id = "clientId-ZvVAsLBrVd";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      // if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
      
//--------------------------------------------Subscribe the topic here.[CanEdit]--------------------------------------------
      client.subscribe("PiaHouse/bedroom/temperature");
      client.subscribe("PiaDevice/ledColour");
      client.subscribe("FRA503/MaxMinNumber");
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while (!Serial) continue;

  //--------------------------------------------Set the pinMode here.[CanEdit]--------------------------------------------
  pinMode(buttonPin, INPUT);
  pinMode(PotentiometerPin, INPUT);
  
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");


  //--------------------------------------------(Publishion zone)_Do someting by depend on your coding and Publish.--------------------------------------------

  //Publishing Example 1: Publish the payload by depent on the logic.
  buttonState = digitalRead(buttonPin);
  if (buttonState == 1) {
    client.publish("PiaHouse/On_Off_PushButton", "OFF");
  } else if (buttonState == 0){
    client.publish("PiaHouse/On_Off_PushButton", "ON");
  }
  
  //Publishing Example 2: Recieve the int value and publish them by convert to char.
  PotentiometerValue = String(analogRead(PotentiometerPin)); //Add String to convert int to String
  Serial.println(PotentiometerValue);
  client.publish("PiaHouse/PotentiometerValue", PotentiometerValue.c_str());
  //Add .c_str to convert String to char because this library uses char, not a string.
  
  
}
