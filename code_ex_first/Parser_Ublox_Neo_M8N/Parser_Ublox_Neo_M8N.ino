#define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#include <WiFi.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
const char* ssid = "elefos";
const char* password = "1234567890a";
//const char* ssid = "PARTH_Labs";
//const char* password = "0905738734";
const char* mqtt_server = "206.189.152.245";
//-----------
String device_id = WiFi.macAddress();
char* pass = "b^sM@p(k3y)20xX";
//-----------
WiFiClient espClient;
PubSubClient client(espClient); 
String letters[62] = {"A", "B", "C", "D", "E", "F","G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z","a", "b", "c", "d", "e", "f","g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
String randString = "";
#include "Ublox.h"
#define N_FLOATS 2
//Ublox::satellite Sate;
Ublox M8_Gps;
float gpsArray[N_FLOATS] = {0, 0};
unsigned long time_set = 0;
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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
//  for (int i = 0; i < length; i++) {
//    tracecoderecive[i] = payload[i];
//    Serial.print((char)payload[i]);
//  }
  Serial.println();
  
//  if (topic == "get_name"){
//  for (int i = 0; i < length; i++) {
//    tracename[i] = payload[i];
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//  }

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("get_tracecode");
      client.subscribe("elefos001");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}
void get_random(){
  for(int i = 0; i<5; i++)
    {
     randString = randString + letters[random(0, 62)];
     Serial.println(randString);
    }
}
void setup()
{
  Serial2.begin(9600);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  get_random();
}
void push_data(){
  //StaticJsonBuffer<200> jsonBuffer;
  //JsonObject& root = jsonBuffer.createObject();
  StaticJsonDocument<200> root;
    root["d"] = device_id;
    root["s"] = randString;
    //root["x"] =  serialized(String(gpsArray[0],7));
   // root["y"] =  serialized(String(gpsArray[1],7));
    root["x"] =  (String(gpsArray[0],7));
    root["y"] =  (String(gpsArray[1],7));
    root["v"] =  M8_Gps.sats[M8_Gps.sats_in_use].snr;
    root["pass"] = pass;
    char jsonChar[200];
    //root.printTo(jsonChar); 
    serializeJson(root,jsonChar); //fuck thuat toan giam so
    client.publish("GPS", jsonChar);
    Serial.println(jsonChar);
    //serializeJsonPretty(jsonChar);
}
void loop()
{

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (!Serial2.available())
    return;

  while (Serial2.available())
  {
    char c = Serial2.read();
    if (M8_Gps.encode(c))
    {
      //gpsArray[0] = M8_Gps.altitude;
        gpsArray[0] = M8_Gps.longitude;
        gpsArray[1] = M8_Gps.latitude;
      //gpsArray[2] = M8_Gps.sats_in_use;
    }
  }
  if( millis() - time_set > 5000){
    time_set = millis();
    push_data();
    Serial.print(gpsArray[0], 7); Serial.print(",");
    Serial.print(gpsArray[1], 7); Serial.println("");
  }
    

}
