#include <SoftwareSerial.h>
SoftwareSerial mySerial(14, 12);           // RX, TX Pins
String apn = "AT+CSTT='v-internet'";                       //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                     //APN-Password
String url = "http://206.189.152.245:1880/gps/update";  //URL for HTTP-POST-REQUEST
//---------------------====
String letters[62] = {"A", "B", "C", "D", "E", "F","G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z","a", "b", "c", "d", "e", "f","g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
String randString = "";
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
#include<ArduinoJson.h>
SFE_UBLOX_GPS myGPS;
#include <WiFi.h>
String device_id = WiFi.macAddress();
char* pass = "b^sM@p(k3y)20xX";
unsigned long time_set = 0;
long lastTime = 0; 
float latitude;
float longitude;
byte SIV = 0;
//----------------------===
void get_random(){
  for(int i = 0; i<5; i++){
     randString = randString + letters[random(0, 62)];
     Serial.println(randString);
    }
}
void setup() {
  Serial.begin(9600);
  // set the data rate for the SoftwaremySerial port
  mySerial.begin(9600);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
  delay(100);
  get_random();
  //--------------------------------------
  while (!Serial); //Wait for user to open terminal
  Serial.println("BusMap testing...");

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

}
void get_data(){
    latitude = myGPS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    longitude = myGPS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    SIV = myGPS.getSIV();
    Serial.print(F(" SIV: ")); // signal
    Serial.print(SIV);
    Serial.println();
}
void push_data(){
  StaticJsonDocument<200> root;
    root["d"] = device_id;
    root["s"] = randString;
    root["x"] =  String(longitude/10000000,7); //chi gui len string de ko bi loi hash
    root["y"] =  String(latitude/10000000,7);
    root["v"] =  SIV;
    root["pass"] = pass;
    char jsonChar[200];
    //root.printTo(jsonChar); 
    serializeJson(root,jsonChar); //fuck thuat toan giam so
    //client.publish("GPS", jsonChar);
    //Serial.println(jsonChar);
    mySerial.println("params=" + String(jsonChar));
    //serializeJsonPretty(jsonChar);
}
void loop() { // run over and over
    //=============
    if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    get_data();
  }
    gsm_sendhttp(); //Start the GSM-Modul and start the transmisson
}


void gsm_sendhttp() {
  
//  mySerial.println("AT");
//  runsl();//Print GSM Status an the Output;
//  delay(4000);
  
  mySerial.println("AT+SAPBR =1,1");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=2,1");
  runsl();
  delay(1000);
  mySerial.println("AT+HTTPINIT");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CID,1");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=URL," + url);
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPDATA=192,1000");
  runsl();
  delay(100);
  push_data();
  //mySerial.println("params=" + data1 + "~" + data2);
  runsl();
  delay(1000);
  mySerial.println("AT+HTTPACTION=1");
  runsl();
  delay(1000);
  mySerial.println("AT+HTTPREAD");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPTERM");
  runsl(); 
}

//Print GSM Status
void runsl() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

}
