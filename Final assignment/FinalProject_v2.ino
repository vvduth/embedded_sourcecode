#include<LiquidCrystal.h>
#include<Wire.h>

LiquidCrystal lcd (12,11,5,4,3,13);

//===pins layout===
#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10

//===interrupts===
#define interrupt_left  18  //Encoder B left
#define interrupt_right 19  //Encoder B right
#define rotationL 23        //Encoder A left
#define rotationR 24        //Encoder A right

//===compass===
#define CMPS14_address 0x60

#define button_input 2
#define input_voltage 18

//===variables===
int analogPin = A0 ;
int analogPin2 = A1;
int power = 0, rotation = 0;
float  value = 0 ;
float voltage = 0 , index_L, index_R ;
String vol_string;
double horiz_divider = 0, vert_divider;
double horiz_num, vert_num;
int pressed_counter = 0 ;
int previous_hor_div, previous_vert_div;
volatile long int pulses = 0, L_counter_prev = 0, R_counter_prev = 0;
float total_brightness = 0;
double index = 0, index_of_power = 0;
volatile bool turnoff = false;
volatile int L_counter, R_counter = 0;
int dist_counter_L, dist_counter_R;
volatile bool carIsMoving = false, forward_L = false, forward_R = false;
int degree;

//variables for compass
byte high_byte = 0;
byte low_byte = 0;
byte angle8 = 0;
unsigned int angle16 = 0;
int heading = 0;
float voltage_pot;

volatile bool stop_var = true; // to define if the robot needs to be stopped


//===========SETUP LOOP=================

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial.println("Put a command to the serial monitor.");

  Wire.begin();
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
  Wire.requestFrom(CMPS14_address, 1, true);

 
  pinMode(button_input,INPUT_PULLUP);
  pinMode(analogPin,INPUT);
  pinMode(analogPin2,INPUT);
  lcd.begin(20,4);
  lcd.print("Motivated");

   //lcd.setCursor(0,1);
   //lcd.print("-100%    0%    +100%");

   //===interrupts===
   attachInterrupt(digitalPinToInterrupt(button_input), buttonpush, FALLING); //interrupt for button
   attachInterrupt(digitalPinToInterrupt(interrupt_left), interruptLeftMotor, RISING); //interrupt for LEFT motor
   attachInterrupt(digitalPinToInterrupt(interrupt_right), interruptRightMotor, RISING); //interrupt for RIGHT motor 
}

//===========END OF SETUP LOOP=================








//===========BEGIN OF MAIN LOOP=================

void loop() {

  //motors power
  int pwm_R=0;
  int pwm_L=0;  

  //displaying positions of cursor on LCD
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
  lcd.setCursor(14,0);
  lcd.print("V:    ");

 
  lcd.setCursor(0,1);
  lcd.print("L:        R:        ");
  lcd.setCursor(3,1);
  lcd.print(L_counter/13.4, 0);
  lcd.setCursor(12,1);
  lcd.print(R_counter/13.4, 0);
 
// ===== COMPASS ======
  //val1 = v/285;
  //val2 = v*360;

  lcd.setCursor(0,2);
  lcd.print("Compass:        ");
  lcd.setCursor(10,2);
  heading = compass_position();
  lcd.print(heading);
  //Serial.print("Raw = ");

  //processing commands from SerialMonitor3 (ESP)
  SerialMonitorCommand();
 
  
  //correlation to "I" position on LCD
  horiz_divider = horiz_num/53,84;
  vert_divider = vert_num/53,84;


  previous_hor_div = horiz_divider;
  previous_vert_div = vert_divider;

  
  index_of_power = 510 - horiz_num;
  if (index_of_power < 0) {
    index_L = 1;
    index_R = (510 - abs(index_of_power))/510;
  }
  else {
    index_R = 1;
    index_L = (510 - abs(index_of_power))/510;
  }


  //defining motors power and direction
  power = 255 - abs(vert_num-3)/2;
  if (power < 0) {
    digitalWrite(Motor_R_dir_pin,0);  
    digitalWrite(Motor_L_dir_pin,1);
  }
  else {
    digitalWrite(Motor_R_dir_pin,1);
    digitalWrite(Motor_L_dir_pin,0);
  }

  analogWrite(Motor_L_pwm_pin,abs(power*index_L));
  analogWrite(Motor_R_pwm_pin,abs(power*index_R));

  delay(150);
  

  L_counter_prev = L_counter;
  R_counter_prev = R_counter;

  carIsMoving = false;


}

//===========END OF MAIN LOOP=================





//====== FUNCTIONS =======

//button push event - to stop the robot
void buttonpush () {

        stopping();
}



//Left motor interrupt
void interruptLeftMotor ()  {
        if (digitalRead(rotationL) == 1) {
          forward_L = true;  //true if the LEFT wheel moves forward
          L_counter++;
        }
        else {
          forward_L = false;
          forward_R = false;
          L_counter--;
        }
}


//Right motor interrupt
void interruptRightMotor () {
     
        if (digitalRead(rotationR) == 1) {
          forward_R = true;  //true if the RIGHT wheel moves forward
          R_counter++;
        }
        else {
          forward_R = false;
          forward_L = false;
          R_counter--;
        }   
        carIsMoving = true;
}


//moving straight at a predefined speed - forward or backward
void moveForward (float dist) {
  dist_counter_L = L_counter;
  dist_counter_R = R_counter;
  
  if (dist < 0) {
    digitalWrite(Motor_R_dir_pin,0);  
    digitalWrite(Motor_L_dir_pin,1);
  }
  else {
    digitalWrite(Motor_R_dir_pin,1);  
    digitalWrite(Motor_L_dir_pin,0);
  }
  sei(); //actovating interrupts
  while (abs(R_counter-dist_counter_R) < 13*(abs(dist)) && abs(L_counter-dist_counter_L) < 13*(abs(dist))) {

    if (abs(R_counter-dist_counter_R) < 0.97*(12.9*(abs(dist))) || abs(L_counter-dist_counter_L) < 0.97*(12.9*(abs(dist))) ||
        abs(R_counter-dist_counter_R) > 0.00*(12.9*(abs(dist))) || abs(L_counter-dist_counter_L) > 0.00*(12.9*(abs(dist)))) {
      analogWrite(Motor_L_pwm_pin,180);
      analogWrite(Motor_R_pwm_pin,180);
    }
    else {
      analogWrite(Motor_L_pwm_pin,220);
      analogWrite(Motor_R_pwm_pin,220);
    }
  }
}



//moving straight at a specified speed until the stop button is pushed - forward or backward
void driving (int dir, int power) {

  if (dir == 0) {
    digitalWrite(Motor_R_dir_pin,0);  
    digitalWrite(Motor_L_dir_pin,1);
  }
  else if (dir == 1) {
    digitalWrite(Motor_R_dir_pin,1);  
    digitalWrite(Motor_L_dir_pin,0);
  }
  
  sei();
  stop_var = false;
  while (!stop_var)
    {
      analogWrite(Motor_L_pwm_pin,power);
      analogWrite(Motor_R_pwm_pin,power);
    }
   
}



//stopping the robot function
void stopping () {
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    stop_var = true;
}




//turning left 90 degrees using pulses - just for testing
void turnLeft () {
  dist_counter_R = R_counter;
    digitalWrite(Motor_R_dir_pin,1);  
    digitalWrite(Motor_L_dir_pin,0);
  stop_var = false;
  sei();
  while (abs(R_counter-dist_counter_R) < 266 && !stop_var) {
    //Serial.println (rotationL);
    //Serial.println(R_counter);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,255);
  }
  analogWrite(Motor_R_pwm_pin,0);
  
}


//turning right 90 degrees using pulses - just for testing
void turnRight () {
  dist_counter_L = L_counter;
    digitalWrite(Motor_R_dir_pin,0);  
    digitalWrite(Motor_L_dir_pin,1);
  sei();
  while (abs(L_counter-dist_counter_L) < 266) {
    analogWrite(Motor_R_pwm_pin,0);
    analogWrite(Motor_L_pwm_pin,255);
  }
}


//defining the current direction using compass raw values
int compass_position ()
{
  float temp=0;
  Wire.begin();
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
  Wire.requestFrom(CMPS14_address, 1, true);

  if (Wire.available() >= 1)
  {
    byte raw_value = Wire.read();
    temp = float(raw_value);
    temp = temp*360;
    temp = temp/255;
  }

  return int(temp);
  
}






//turn RIGHT by amount of degrees
void turn_x_degrees_R(int deg, int initial){
  //compass_position();
  int initial_angle = initial, current_angle = 0, passed=0;
  Serial.print("Initial: ");
  Serial.println(initial_angle);
  
  digitalWrite(Motor_R_dir_pin,1);  
  digitalWrite(Motor_L_dir_pin,0);
  sei();
  stop_var = false;
  
  while(passed<deg && !stop_var){
    //Serial.println(stop_var);
    analogWrite(Motor_R_pwm_pin,0);
    analogWrite(Motor_L_pwm_pin,150);
    current_angle = compass_position();
    if ((current_angle-initial_angle) >= 0)
     {
      passed = current_angle -initial_angle;
      passed = passed*1.04;
     }
    else
     {
      passed = (360 - initial_angle  + current_angle);
      passed = passed*1.04;
     }
     delay(20);
  }
  analogWrite(Motor_L_pwm_pin,0);
  Serial.print("Current: ");
  Serial.println(current_angle);
  
}




//turn LEFT by amount of degrees
void turn_x_degrees_L(int deg, int initial){
  //compass_position();
  int initial_angle = initial, current_angle = 0, passed=0;
  Serial.print("Initial: ");
  Serial.println(initial_angle);
  
  digitalWrite(Motor_R_dir_pin,1);  
  digitalWrite(Motor_L_dir_pin,0);
  sei();
  stop_var = false;
  
  while(passed<deg && !stop_var){
    analogWrite(Motor_R_pwm_pin,150);
    analogWrite(Motor_L_pwm_pin,0);
    current_angle = compass_position();
    if ((initial_angle - current_angle) >= 0)
     {
      passed = initial_angle - current_angle;
      passed = passed*1.04;
     }
    else
     {
      passed = (360 - current_angle + initial_angle);
      passed = passed*1.04;
     }
     delay(20);
  }
  analogWrite(Motor_L_pwm_pin,0);
  Serial.print("Current: ");
  Serial.println(current_angle);
  
}



//turn to specific direction - we just put required DEGREE 0-360 and choose the best direction - clockwise or CCW
//=====NOT USED at the moment===== JUST FOR TESTING
void turn_specific_direction(int deg, int initial)
{
  
  int initial_angle = initial, final_angle = deg, passed=0, difference = 0;

  if ((initial_angle - final_angle) < 0 )
  {
    difference = 360 - (final_angle - initial_angle);
  }
  else difference = initial_angle - final_angle;

  if (difference < 180)  //then turn left
  {
    digitalWrite(Motor_R_dir_pin,1);  
    digitalWrite(Motor_L_dir_pin,0);
    sei();
  
    while(abs(compass_position() - (deg)) > 4){
      //Serial.print("Current: ");
      //Serial.println(current);
      analogWrite(Motor_R_pwm_pin,150);
      analogWrite(Motor_L_pwm_pin,0);
      delay(10);
    }

    
  }
  
  else //then turn right
  {
    digitalWrite(Motor_R_dir_pin,1);  
    digitalWrite(Motor_L_dir_pin,0);
    sei();
  
    while(abs(compass_position() - (deg)) > 4){
      //Serial.print("Current: ");
      //Serial.println(current);
      analogWrite(Motor_R_pwm_pin,0);
      analogWrite(Motor_L_pwm_pin,150);
      delay(10);
    }
  }
  
}



//processing commands on the Serial Monitor
void SerialMonitorCommand()
{
  //Checking Serial Monitor 3 (ESP)
  if (Serial3.available() > 0){
    String message = Serial3.readStringUntil('\n'); 
    Serial3.print("Message received, content: ");  
    Serial3.println(message);
    Serial.print("Message from ESP: ");
    Serial.println(message);
    bool double_command = false;
   

    //checking double command Move+Speed
    int pos_s = message.indexOf(",");
    if  (pos_s > -1){
      double_command = true;
      String substr1 = message.substring(0, pos_s);
      Serial.println(substr1);
      String substr2 = message.substring(pos_s + 1);
      Serial.println(substr2);
      
      //Looking for ":" in each of both substrings
      int pos_1 = substr1.indexOf(":");
      int pos_2 = substr2.indexOf(":");

      if (pos_1 > -1 && pos_2 > -1){
        Serial.println("Command = Move + Speed");
        pos_1 = substr1.indexOf(":");
        pos_2 = substr2.indexOf(":");
  
        if (pos_1 > -1 && pos_2 > -1){
          String stat1 = substr1.substring(pos_1 + 1);
          String stat2 = substr2.substring(pos_2 + 1);
          Serial.print("Direction: ");
          Serial.println(stat1);
          Serial.print("Speed: ");
          Serial.println(stat2);
          
          //direction
          int dir = stat1.toInt();
          //speed
          int power = stat2.toInt();
          
          driving(dir, power);
        }   
      }
    }


    if (!double_command)
    {

      //checking PWM command
      pos_s = message.indexOf("Move");
  
      if (pos_s > -1){
        Serial.println("Command = Move ");
        pos_s = message.indexOf(":");
  
        if (pos_s > -1){
          String stat = message.substring(pos_s + 1);
          int driveforward = stat.toInt();
          moveForward(driveforward);
        
        }  
      }


    
      pos_s = message.indexOf("Turn");
  
      if (pos_s > -1){
        Serial.println("Command = Turn ");
        pos_s = message.indexOf(":");
        Serial.println(pos_s);
  
        if (pos_s > -1){
          String stat = message.substring(pos_s + 1);
          int angle = stat.toInt();
          if (angle > 0) {
            turn_x_degrees_R(angle, heading);
            //turnRight();
          }
          else {
            turn_x_degrees_L(abs(angle), heading);
            //turnLeft();
            Serial.println(angle);
  
          }
        }  
      }
    }

  }


 /*
   //not using SM commands on Arduino at the moment
  //Checking Serial Monitor 1 (Arduino)
  if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n'); 
    Serial.print("Message received, content: ");  
    Serial.println(message);

    //checking double command Move+Speed
    int pos_s = message.indexOf("&");
    if  (pos_s > -1){
      String substr1 = message.substring(0, pos_s);
      Serial.println(substr1);
      String substr2 = message.substring(pos_s + 1);
      Serial.println(substr2);
      
      //Looking for ":" in each of both substrings
      int pos_1 = substr1.indexOf(":");
      int pos_2 = substr2.indexOf(":");

      if (pos_1 > -1 && pos_2 > -1){
        Serial.println("Command = Move + Speed");
        pos_1 = substr1.indexOf(":");
        pos_2 = substr2.indexOf(":");
  
        if (pos_1 > -1 && pos_2 > -1){
          String stat1 = substr1.substring(pos_1 + 1);
          String stat2 = substr2.substring(pos_2 + 1);
          Serial.println(stat1);
          Serial.println(stat2);
          
          //direction
          int dir = stat1.toInt();
          //speed
          int power = stat2.toInt();
          
          driving(dir, power);
        }   
      }
    }

    //if it's NOT double command on SM Arduino
    else {

      pos_s = message.indexOf("Turn");

      if (pos_s > -1){
        Serial.println("Command = Turn ");
        pos_s = message.indexOf(":");
        Serial.println(pos_s);
  
        if (pos_s > -1){
          String stat = message.substring(pos_s + 1);
          int angle = stat.toInt();
          if (angle > 0) turn_x_degrees_R(angle, heading);
          else {
            turn_x_degrees_L(abs(angle), heading);
            Serial.println(angle);
  
          }
        }  
      }
      }
  }

  */
}
