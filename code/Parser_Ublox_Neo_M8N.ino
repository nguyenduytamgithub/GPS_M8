#include "Ublox.h"
#define N_FLOATS 3

Ublox M8_Gps;
float gpsArray[N_FLOATS] = {0, 0, 0};


#include <SoftwareSerial.h>
//SoftwareSerial mySerial(RX,TX);
SoftwareSerial mySerial(9, 10);



void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop()
{
  if (!mySerial.available())
    return;

  while (mySerial.available())
  {
    char c = mySerial.read();
    if (M8_Gps.encode(c))
    {
      //gpsArray[0] = M8_Gps.altitude;
      gpsArray[0] = M8_Gps.latitude;
      gpsArray[1] = M8_Gps.longitude;
      gpsArray[2] = M8_Gps.sats_in_use;
    }
  }
  for (byte i = 0; i < 3; i++)
  {
    if (i == 2)
    {
      Serial.print(gpsArray[i], 0); Serial.println("");
    }
    else
    {
      Serial.print(gpsArray[i], 6); Serial.print("_");
    }
  }
}

