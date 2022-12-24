#include "PMS.h"
#include "SoftwareSerial.h"
#include <TFT.h>  
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

double a1, b2, c10; 
SoftwareSerial Serial1(4, 5); // RX, TX
TFT TFTscreen = TFT(cs, dc, rst);
 
PMS pms(Serial1);
PMS::DATA data;
 
void setup()
{
  Serial1.begin(9600);
  Serial.begin(9600);
  TFTscreen.begin();

  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(127,127,127);

  TFTscreen.line(0,18,160,18);
  TFTscreen.line(80,18,80,128);
  TFTscreen.line(80,73,160,73);
  
  TFTscreen.setTextSize(19/10);
  TFTscreen.stroke(255,215,0);
  TFTscreen.text("ECO LOGIC", 50,5);
  
  TFTscreen.setTextSize(9/5);
  TFTscreen.stroke(0,0,255);
  TFTscreen.text("PM2.5", 25,30);
  TFTscreen.text("ug/m3", 25,110);
  TFTscreen.text("PM1.0", 108,20);
  TFTscreen.text("PM10", 108,75);
}
 
void loop(){
  if (pms.read(data))
  {
   //Serial.println("Dust Concentration");
   a1 = data.PM_AE_UG_1_0;
   char txa1[5];
   dtostrf(a1, 1, 0, txa1);
   b2 = data.PM_AE_UG_2_5;
   char txb2[5];
   dtostrf(b2, 1, 0, txb2);
   c10 = data.PM_AE_UG_10_0;
   char txc10[5];
   dtostrf(c10, 1, 0, txc10);
   
   //Serial.println("PM1.0: " + String(a1) + "(ug/m3)");
   //Serial.println("PM2.5: " + String(b2) + "(ug/m3)");
   //Serial.println("PM10: " + String(c10) + "(ug/m3)");
   //delay(2000);

  TFTscreen.setTextSize(3);
  TFTscreen.stroke(255, 0, 255);
  TFTscreen.text(txa1,100,40);

  TFTscreen.stroke(0,255,0);
  TFTscreen.text(txb2,20,65);
  
  TFTscreen.stroke(255,192,203);
  TFTscreen.text(txc10,100,95);

  delay(2000);
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(txa1, 100,40);
  
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(txb2, 20,65);
  
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(txc10, 100,95);
  
  Serial.println(txa1);
  Serial.println(txb2);
  Serial.println(txc10);
  }
}
