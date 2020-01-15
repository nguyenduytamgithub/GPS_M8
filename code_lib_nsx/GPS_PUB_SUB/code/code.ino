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
unsigned long time_set = 0;
//-----------
WiFiClient espClient;
PubSubClient client(espClient); 
String letters[62] = {"A", "B", "C", "D", "E", "F","G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z","a", "b", "c", "d", "e", "f","g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
String randString = "";
//-------------------
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount of I2C traffic to Ublox module.
float latitude;
float longitude;
byte SIV = 0;
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
  for(int i = 0; i<5; i++){
     randString = randString + letters[random(0, 62)];
     Serial.println(randString);
    }
}
void setup()
{
  //--------------------------------------
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  get_random();
  //--------------------------------------
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");

  //Assume that the U-Blox GPS is running at 9600 baud (the default) or at 38400 baud.
  //Loop until we're in sync and then ensure it's at 38400 baud.
  do {
    Serial.println("GPS: trying 38400 baud");
    Serial2.begin(38400);
    if (myGPS.begin(Serial2) == true) break;

    delay(100);
    Serial.println("GPS: trying 9600 baud");
    Serial2.begin(9600);
    if (myGPS.begin(Serial2) == true) {
        Serial.println("GPS: connected at 9600 baud, switching to 38400");
        myGPS.setSerialRate(38400);
        delay(100);
    } else {
        //myGPS.factoryReset();
        delay(2000); //Wait a bit before trying again to limit the Serial output
    }
  } while(1);
  Serial.println("GPS serial connected");

  myGPS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR
  //------------------------------------------------
  
}
void get_data(){
    latitude = myGPS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    longitude = myGPS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

//    long altitude = myGPS.getAltitude();
//    Serial.print(F(" Alt: "));
//    Serial.print(altitude);
//    Serial.print(F(" (mm)"));

    SIV = myGPS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();
}
void push_data(){
  StaticJsonDocument<200> root;
    root["d"] = device_id;
    root["s"] = randString;
    //root["x"] =  serialized(String(gpsArray[0],7));
    //root["y"] =  serialized(String(gpsArray[1],7));
    // root["x"] =  (String(gpsArray[0],7));
    // root["y"] =  (String(gpsArray[1],7));
    // root["v"] =  M8_Gps.sats[M8_Gps.sats_in_use].snr;
    root["x"] =  String(longitude/10000000,7); //chi gui len string de ko bi loi hash
    root["y"] =  String(latitude/10000000,7);
    root["v"] =  SIV;
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
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    get_data();
  }
  if( millis() - time_set > 5000){
    time_set = millis();
    push_data();
  }
}
