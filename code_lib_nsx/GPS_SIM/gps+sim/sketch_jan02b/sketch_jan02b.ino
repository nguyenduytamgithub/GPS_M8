#include <SoftwareSerial.h>

String Arsp, Grsp;
SoftwareSerial gsm(14, 12); // RX, TX
String apn = "AT+CSTT='v-internet'";                       //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                     //APN-Password
String url = "http://206.189.152.245:1880/gps/update";  //URL for HTTP-POST-REQUEST
String data1 = "123";
String data2 = "234";
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Testing GSM SIM800L");
  gsm.begin(9600);
  gsm.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  gsm.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:

//  if(gsm.available())
//  {
//    Grsp = gsm.readString();
//    Serial.println(Grsp);
//  }
//
//  if(Serial.available())
//  {
//    Arsp = Serial.readString();
//    gsm.println(Arsp);
//  }
gsm_sendhttp();
delay(1000);
}
void gsm_sendhttp() {
  
//  gsm.println("AT");
//  runsl();//Print GSM Status an the gsm Output;
//  delay(4000);
//  
  
  gsm.println("AT+SAPBR=2,1");
  runsl();
  delay(1000);
  gsm.println("AT+HTTPINIT");
  runsl();
  delay(100);
  gsm.println("AT+HTTPPARA=CID,1");
  runsl();
  delay(100);
  gsm.println("AT+HTTPPARA=URL," + url);
  runsl();
  delay(100);
  gsm.println("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  runsl();
  delay(100);
  gsm.println("AT+HTTPDATA=192,10000");
  runsl();
  delay(10000);
 // push_data();
  gsm.println("params=" + data1 + "~" + data2);
  //delay(1500);
  runsl();
  delay(1000);
  gsm.println("AT+HTTPACTION=1");
  runsl();
  delay(100);
  gsm.println("AT+HTTPREAD");
  runsl();
  delay(100);
  gsm.println("AT+HTTPTERM");
  runsl(); 
}

//Print GSM Status
void runsl() {
  while (gsm.available()) {
    Serial.write(gsm.read());
  }

}
