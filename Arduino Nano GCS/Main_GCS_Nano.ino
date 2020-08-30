#define front_joystick_pin A0
#define side_joystick_pin A1
#define arming_switch_pin 6
#define uv_switch_pin 5

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>                   //Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";    // NRF24L01 Address
char textin[32] = "";
int c1,c2;

int front_pot = 0;                  //Value of forward backward potentiometer of the joystick
int side_pot = 0;                   //Value of left right potentiometer of the joystick
bool robot_arm_switch = 0;          //Value of the robot arming switch
bool uv_switch = 0;                 //Value of the UV switch

char cmd[33];
int pos = 0;                        //counter variable for characters

 
void setup() {
  radio.begin();                      //Radio module init
  radio.openWritingPipe(address);     //Assign the address
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.begin(115200);               //Serial monitor 

  lcd.begin(16, 2);
  lcd.setRGB(0, 200, 0);              //Set backlight Color (R,G,B)
  lcd.print("UV Robot V2.0");

  pinMode(front_joystick_pin, INPUT);
  pinMode(side_joystick_pin, INPUT);
  pinMode(arming_switch_pin, INPUT);
  pinMode(uv_switch_pin, INPUT);
}


void loop() {

  lcd_message_print(); //print received messages from the robot on LCD
  switch_read();

  //sends navigation commands to the robot only is the robot is armed
  if (robot_arm_switch == 1){
    control_commands();
  }

  else{
    system_cutoff();
  }
                                                      
}


void switch_read(){

    front_pot = analogRead(front_joystick_pin);
    side_pot = analogRead(side_joystick_pin);
    robot_arm_switch = digitalRead(arming_switch_pin);
    uv_switch = digitalRead(uv_switch_pin);
  
}

void lcd_message_print(){
  radio.read(&textin, sizeof(textin));    //read the messages received from the robot
  int cin = textin[0];                    //received command

  //Decode the message

  //obstacle detected in front
  if (cin == 10){
    lcd.setRGB(100, 100, 0); 
    lcd.setCursor(0, 1);
    lcd.print("Front blocked   ");
    delay(400);
  }

  //obstacle detected in left
  if (cin == 20){
    lcd.setRGB(100, 100, 0); 
    lcd.setCursor(0, 1);
    lcd.print("Left Blocked    ");
    delay(400);
  }

  //obstacle detected in right
  if (cin == 30){
    lcd.setRGB(100, 100, 0); 
    lcd.setCursor(0, 1);
    lcd.print("Right blocked   ");
    delay(400);
  }

  //obstacle detected in rear
  if (cin == 40){
    lcd.setRGB(100, 100, 0); 
    lcd.setCursor(0, 1);
    lcd.print("Rear blocked    ");
    delay(400);
  }

  //Battery low
  if (cin == 50){
    lcd.setRGB(200, 0, 0); 
    lcd.setCursor(0, 1);
    lcd.print("Battery Low     ");
    delay(400);
  }

  //Human Detected and UV lights are off
  if (cin == 60){
    lcd.setRGB(100, 0, 100); 
    lcd.setCursor(0, 1);
    lcd.print("Human Detected  ");
    delay(400);
  }
}

void control_commands(){
   if (side_pot > 800) c1 = 400;          //turn left
   else if (side_pot < 200) c1 = 300;     //turn right
   else if (front_pot > 800) c1 = 100;    //go forward
   else if (front_pot < 200) c1 = 200;   //go backward

   if (uv_switch == 1) c2 = 100;
   else c2 = 200;

  char text[] = "0000   ";
  text[0] = c1;
  text[1] = c2;                          // Append Data to text
  radio.write(&text, sizeof(text));       // Send commands to the robot
}

void system_cutoff(){

  c1 = 50;
  c2 = 200;

  char text[] = "0000   ";
  text[0] = c1;
  text[1] = c2;                          // Append Data to text
  radio.write(&text, sizeof(text));       // Send commands to the robot
}
