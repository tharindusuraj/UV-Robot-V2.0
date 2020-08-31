#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <nRF24L01.h>   //Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <RF24.h>
#include <EEPROM.h>

RF24 radio(PB0, PA4); // CE, CSN
const byte address[][6] = {"00001","00002"};    
int addr = 0;

#define OLED_MOSI   PB6 //d1
#define OLED_CLK   PB7 //d0
#define OLED_DC    PB5
#define OLED_CS    PB4
#define OLED_RESET PB3
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//defining PIR pins
#define PIR1 PA8
#define PIR2 PA9
#define PIR3 PA10
#define PIR4 PA12

#define buzzer PA11
#define RED_LED PB10
#define GREEN_LED PB11
#define bat_voltage_pin PB1
#define uv_relay_pin PA15

//defining motor controller pins
#define M2CC PA3
#define M2CW PA2
#define M1CC PA1
#define M1CW PA0

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define MAX_ROBOT_SPEEDR 120     //Right Motor Speed
#define MAX_ROBOT_SPEEDL 120     //Left Motor Speed

float v_sense;                  //battery voltage

char text[32] = "";             //Received NRF messages
char textout[32] = "";          //NRF messages to be sent

int cout;                       //commands sent to the GCS
int c1,c2;                      //commands received from the GCS

bool uv_on_flag = 0;
bool human_detected_flag = 0;

int obstacle_status[4];         //obstacle status of (front. left, rear, right)

unsigned long t1,t2,t3,t4;      //millis timer variables

int LAMP_MIN,LAMP_HOUR;
int lamp_min_addr = 1;            //EEPROM memory address
int lamp_hour_addr = 10;

void setup()   { 
  radio.begin();
  radio.openReadingPipe(0,address[0]);
  radio.openWritingPipe(address[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening(); 
  Serial.begin(115200);

  pinMode( bat_voltage_pin,INPUT); 
  pinMode (uv_relay_pin,OUTPUT); 
  pinMode (GREEN_LED,OUTPUT); 
  pinMode (RED_LED,OUTPUT); 
  
  pinMode (M2CC,OUTPUT);
  pinMode (M2CW, OUTPUT);
  pinMode (M1CC,OUTPUT);
  pinMode (M1CW, OUTPUT);
  
  pinMode (buzzer,OUTPUT);  

  pinMode (PIR1,INPUT);
  pinMode (PIR2,INPUT);
  pinMode (PIR3,INPUT);
  pinMode (PIR4,INPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event

  LAMP_HOUR = readFromEEPROM(lamp_hour_addr);        //get the lamp minutecount from EEPROM
  LAMP_MIN  = readFromEEPROM(lamp_min_addr);

  delay(30000);                        //wait 30 seconds to initialize PIR
  tone(buzzer,300);                   //short beep after initialization
  delay(500);
  noTone(buzzer);

}
void loop(){

bat_voltage_read();
pir_check();
UV_control();
nrf();
read_received_commands();
oled ();
Cal_Lamp_Hours();
}

//This will trigger automatically when the event occurs
void receiveEvent(int howMany){
  int i = 0;
  while(1 < Wire.available()){ // loop through all but the last
    int c = Wire.read(); // receive byte as a character
    obstacle_status[i] = c;
    i++;
  }
}
