
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>                   //Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";    // NRF24L01 Address
char textin[32] = "";
int cin;                            //input command
int c1,c2,c3,c4;

char cmd[33];
int pos = 0;                        //counter variable for characters

 
void setup() {
  radio.begin();                      //Radio module init
  radio.openWritingPipe(address);     //Assign the address
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.begin(115200);               //Serial monitor 
}


void loop() {


  while(Serial.available()) {
    char c = Serial.read();           //read the serial input

    //When a line break is detected reset the counter variable to 0 and intepret the command
    if (c == ';' || c == '\n') {      
      cmd[pos] = '\0';
      InterpretCommand(cmd);
      pos = 0;
    } 
    else {
        cmd[pos++] = c;
    }
  }
  
  char text[] = "0000   ";
  text[0] = c1;
  text[1] = c2;
  text[2] = c3;
  text[3] = c4;                           // Append Data to text
  radio.write(&text, sizeof(text));       // Send commands to the robot
                             
  radio.read(&textin, sizeof(textin));    //read the messages received from the robot
  cin = textin[0];

  //Decode the message

  if (cin == 10){
    Serial.println("Obstacal detected FRONT");
  }
  if (cin == 20){
    Serial.println("Obstacal detected LEFT");
  }
  if (cin == 30){
    Serial.println("Obstacal detected RIGHT");
  }
  if (cin == 40){
    Serial.println("Obstacal detected REAR");
  }
  if (cin == 50){
    Serial.println("Low Battery");
  }
  if (cin == 60){
    Serial.println("Human detected !");
  }
                             
}

void InterpretCommand(char* cmd) {
    char c;
    float f;
    int num_parsed = sscanf(cmd, " %c %f", &c, &f);
    if (num_parsed < 1) {
        Serial.println("Invalid command");
        return;
    }
    switch(c) {
        // Change gait frequency
        case 'F':
            Serial.println ("Going Forward");
            c1 = 100;
            break;
        case 'r':
            Serial.println ("Going reverse");
            c1 = 200;
            break;
        case 'R':
            Serial.println ("Going right");
            c1 = 300;
            break;
        case 'L':
            Serial.println ("Going left");
            c1 = 400;
             break;
        case 'Q':
            Serial.println ("UV ON");
            c2 = 100;
            break;
        case 'q':
            Serial.println ("UV OFF");
            c2 = 200;
            break;
         case 'S':
            Serial.println ("STOP");
            c1 = 50;
            break;
        default:
            Serial.println("Unknown command");
    }
}
