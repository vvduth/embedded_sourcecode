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
  lcd.print("Motivated");
   //pinMode(2, OUTPUT);
   //DDRD = B00000111;
   //Bitwise operation instead (not touching pins 3-7):
   DDRD = DDRD | B00000111;
   Serial.begin(9600);
   lcd.setCursor(0,1);
}

void loop() {
 
  //PORTD = B00000001;
  //Bitwise operation instead (not touching other pins) - HIGH value at pin 0:
  PORTD = PORTD | B00000001;
  delay(1000);

  //PORTD = B00000010;
  //clearing out pin 0 with others pins unchanged - LOW value at pin 0:
  PORTD = PORTD & B11111110;
  //switching pin 1 to HIGH - HIGH value at pin 1;
  PORTD = PORTD | B00000010;
  delay(1000);
  //PORTD = B00000100;
  //clearing out pin 1 with others pins unchanged - LOW value at pin 1:
  PORTD = PORTD & B11111101;
  //switching pin 2 to HIGH:
  PORTD = PORTD | B00000100;
  delay(1000);
  //clearing out pin 2 with others pins unchanged:
  PORTD = PORTD & B11111011;
  
}
