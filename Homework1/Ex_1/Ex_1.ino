#include<LiquidCrystal.h>

LiquidCrystal lcd (8,7,6,5,4,3 );

int analogPin = A0 ;
int analogPin2 = A1;
float value = 0 ;
float voltage = 0 ;
float megaVal ;
float megaVol ;
String vol_string;

void setup() {
  pinMode(analogPin,INPUT);
  pinMode(analogPin2,INPUT);
  lcd.begin(20,4);
  lcd.print("Motivated");
  Serial.begin(9600);
   
}

void loop() {
  lcd.setCursor(0,1);
  //Reading value at A0
  value = analogRead(analogPin);
  delay(500);
  lcd.setCursor(1,3);
  
  voltage = value*5/1023;

  //Reading value at A1
  megaVal = analogRead(analogPin2);
  
  lcd.setCursor(1,1);
  lcd.print("Poten Vol:");
  lcd.setCursor(11,1);
  lcd.print(voltage);

  megaVol = megaVal*5/1023 ;
  lcd.setCursor(1,2);
  lcd.print("Mega vol:");
  lcd.setCursor(11,2);
  lcd.print(megaVol);
  
}
