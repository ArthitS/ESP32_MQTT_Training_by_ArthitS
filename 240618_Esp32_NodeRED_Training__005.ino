#include <WiFi.h>           //Library WiFi for esp32
#include <PubSubClient.h>
#include <ArduinoJson.h>    //https://arduinojson.org/

//--------------------------------------------Variable Declaration[CanEdit]--------------------------------------------

//Variable for Subscription Example 1 --> Topic: "PiaHouse/bedroom/temperature"
double temperature;

//Variable for Subscription Example 3--> Topic: "PiaIOT/Subscript_JsonFormat"
String Json_Sub;
const char* sensor;
long time1;
double latitude;
double longitude;

//Variable for Publishing Example 1 --> Topic: "PiaHouse/On_Off_PushButton"
int buttonPin = 23;
int buttonState = 0;

//Variable for Publishing Example 2 --> Topic: "PiaHouse/PotentiometerValue"
int PotentiometerPin = 34;
String PotentiometerValue;

//Variable for Publishing Example 3 --> Topic: "PiaIOT/Publish_JsonFormat"
String Json_Pub;
double tempValue = 24;



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
  else if (topic == "PiaIOT/Subscript_JsonFormat"){
    //TestCase: {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
    Serial.println(messageTemp);

    Json_Sub = messageTemp;
    JsonDocument doc;
    
    deserializeJson(doc, Json_Sub);

    sensor    = doc["sensor"];
    time1     = doc["time"];
    latitude  = doc["data"][0];
    longitude = doc["data"][1];

    //Show the data before deserialize
    Serial.println(sensor);
    Serial.println(time1);
    Serial.println(latitude); 
    Serial.println(longitude); 
    
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
      client.subscribe("PiaIOT/Subscript_JsonFormat");
    
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
  
  //Publishing Example 2: Recieve the int value, convert int to string, and publish them by convert to char.
  PotentiometerValue = String(analogRead(PotentiometerPin)); //Add String() to convert int to String
  //Serial.println(PotentiometerValue);
  client.publish("PiaHouse/PotentiometerValue", PotentiometerValue.c_str());  //Add .c_str to convert String to char because this library uses char, not a string.
  
  
  //Publishing Example 3: Recieve the multiple data, Generate JSON, and publish them by convert to char.
  if(Serial.read() == 'j'){
    JsonDocument PiaDoc;
    
    PiaDoc["temperature"] = tempValue;
    PiaDoc["key"] = "value";
    PiaDoc["raw"] = serialized("[1,2,3]");
    PiaDoc["data"][0] = 48.756080;
    PiaDoc["data"][1] = 2.302038;

    serializeJson(PiaDoc, Json_Pub);
    Serial.print(Json_Pub);
    client.publish("PiaIOT/Publish_JsonFormat", Json_Pub.c_str());
    // This publish should be: {"temperature":24,"key":"value","raw":[1,2,3],"data":[48.75608,2.302038]}
    } 
  
}
