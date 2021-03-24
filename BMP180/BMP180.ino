/* SFE_BMP180 library example sketch

- (GND) to GND
+ (VDD) to 3.3V

Pin:                SDA  SCL
Arduino Uno:        A4   A5
NodeMCU:            D2   D1

SFE_BMP180 kütüphanesine ait örnekten yararlanılmıştır.
24/03/2021

*/


#include <SFE_BMP180.h>
#include <Wire.h>


SFE_BMP180 pressure;

#define ALTITUDE 8.0 // Altitude of ISKENDERUN. (İskenderun'a ait RAKIM değeri)

void setup()
{
  Serial.begin(9600);
  Serial.println("REBOOT");


  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}

void loop()
{
  char status;
  double T,P,p0,a;
  
  Serial.println();
  Serial.print("RAKIM: ");
  Serial.print(ALTITUDE,0);
  Serial.print("metre, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println("feet");

  status = pressure.startTemperature();
  if (status != 0)
  {
    
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {

      Serial.print("SICAKLIK: ");
      Serial.print(T,2);
      Serial.print(" C derece, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" Fahrenhayt");
      

      status = pressure.startPressure(3);
      if (status != 0)
      {

        delay(status);

        status = pressure.getPressure(P,T);
        if (status != 0)
        {

          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          p0 = pressure.sealevel(P,ALTITUDE); 
          Serial.print("RELATIV BASINC (Deniz Seviyesindeki): ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          a = pressure.altitude(P,p0);
          Serial.print("HESAPLANAN RAKIM: ");
          Serial.print(a,0);
          Serial.print(" metre, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(5000);
}
