#include<LiquidCrystal.h>

LiquidCrystal lcd (8,7,6,5,4,3 );

#define led1 9
#define led2 10


#define button_input 19
int analogPin = A0 ;
int analogPin2 = A1;
float  value = 0 ;
float voltage = 0 ;
String vol_string;
double horiz_divider = 0, vert_divider;
double horiz_num, vert_num;
int pressed_counter = 0 ;
int previous_hor_div, previous_vert_div;
volatile long int pulses = 0;
float total_brightness = 0;
double index = 0;
volatile bool turnoff = false;


void setup() {
  //DDRD = DDRD | B00000111;
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  pinMode(button_input,INPUT_PULLUP);
  pinMode(analogPin,INPUT);
  pinMode(analogPin2,INPUT);
  lcd.begin(20,4);
  lcd.print("Motivated");

   Serial.begin(9600);

   lcd.setCursor(0,1);
   lcd.print("-100%    0%    +100%");
    
   attachInterrupt(digitalPinToInterrupt(button_input), frequencyCounter, FALLING); //interrupt for button
   
}

void loop() {


  lcd.setCursor(0,0);
  lcd.print("X:");
  horiz_num = analogRead(A0);  //x-axis
  vert_num = analogRead(A1);  //y-axis
  lcd.print(horiz_num, 0);
  lcd.print("     ");
  lcd.setCursor(7,0);
  lcd.print("Y:    ");
  lcd.setCursor(9,0);
  lcd.print(vert_num, 0);
  lcd.setCursor(15,0);
  lcd.print("B:   ");

  lcd.setCursor(17,0);
  pressed_counter = pulses;
  lcd.print(pressed_counter);

  //correlation to "I" position
  horiz_divider = horiz_num/53,84;
  vert_divider = vert_num/53,84;

if (turnoff) {
  analogWrite(led2,0);
  analogWrite(led1,0);
}
else {
  
  total_brightness = (1023-vert_num) / 4;
  index = horiz_num/1023; //along X-axis
  analogWrite(led2, total_brightness*index ); // dimming LED2
  analogWrite(led1, total_brightness - total_brightness*index ); // dimming LED1
}

//changing "I" position ONLY if it's different from previous iteration of the main loop
if (previous_hor_div != horiz_divider) {
  //clearing out the line
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(horiz_divider,2);
  lcd.print('I');
}


//changing "I" position ONLY if it's different from previous iteration of the main loop
if (previous_vert_div != vert_divider) {
  lcd.setCursor(0,3);
  lcd.print("                    ");
  lcd.setCursor(vert_divider,3);
  lcd.print('I');
}

  previous_hor_div = horiz_divider;
  previous_vert_div = vert_divider;

  delay(150);
 
}



void frequencyCounter () {
    pulses++;
    turnoff = !turnoff;
}
