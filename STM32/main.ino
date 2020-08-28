
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <nRF24L01.h>   //Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <RF24.h>

 RF24 radio(PB0, PA4); // CE, CSN
 const byte address[6] = "00001";

 int c1,c2,c3,c4;
    

// If using software SPI (the default case):
#define OLED_MOSI   PB6 //d1
#define OLED_CLK   PB7 //d0
#define OLED_DC    PB5
#define OLED_CS    PB4
#define OLED_RESET PB3
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define buzzer PA11
#define RED_LED PB10
#define GREEN_LED PB11
#define M2CC PA3
#define M2CW PA2
#define M1CC PA1
#define M1CW PA0
#define OBS_DATA PB9
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define MAX_ROBOT_SPEED 150     //PWM value
int Obstacal = 0;
float v_sense;
int Relay = 0;
char text[32] = "";

void setup()   { 
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening(); 
  Serial.begin(115200);
  pinMode(  PB1,INPUT); 
  pinMode (PA8,OUTPUT); 
  pinMode (GREEN_LED,OUTPUT); 
  pinMode (RED_LED,OUTPUT); 
  
  pinMode (M2CC,OUTPUT);
  pinMode (M2CW, OUTPUT);
  pinMode (M1CC,OUTPUT);
  pinMode (M1CW, OUTPUT);
  
  pinMode (PA11,OUTPUT);  
  pinMode (OBS_DATA,INPUT);
          
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
}
void loop(){
  if (digitalRead(OBS_DATA) == HIGH){
digitalWrite (RED_LED,HIGH);
Obstacal = 1;
  }
  else {
    digitalWrite (RED_LED,LOW);
Obstacal = 0;
  }

NRF();
disp();
moter_drive();
 Oled ();
 RELAY();
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
 }
