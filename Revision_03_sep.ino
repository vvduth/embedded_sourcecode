#include<LiquidCrystal.h>

LiquidCrystal lcd (8,7,6,5,4,3 );

int analogPin = A0 ;
int analogPin2 = A1;
float  value = 0 ;
float voltage = 0 ;
float megaVal ;
float megaVol ;
String vol_string;
void setup() {
  pinMode(analogPin,INPUT);
  lcd.begin(20,4);
  //lcd.print("Motivated");
   //pinMode(2, OUTPUT);
   DDRD = B00000111;
   Serial.begin(9600);
   
}

void loop() {
  lcd.setCursor(0,1);
  digitalWrite(2, HIGH);
  PORTD = B00000001;
  
  
  delay(1000);

  PORTD = B00000010;
  delay(1000);
  PORTD = B00000100;
  delay(1000);
  


  value = analogRead(analogPin);
  delay(500);
  lcd.setCursor(1,3);
  
   //voltage = value*5/1023 ;

   //megaVal = analogRead(analogPin2);
  
  
  /*lcd.setCursor(1,1);
  lcd.print("Poten Vol:");
  lcd.setCursor(11,1);
  lcd.print(voltage);

  megaVol = megaVal*5/1023 ;
   lcd.setCursor(1,2);
  lcd.print("Mega Vol:");
  lcd.setCursor(11,2);
  lcd.print(megaVol);*/
  
}
