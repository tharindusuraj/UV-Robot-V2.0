void oled (){

  //display header
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("UV_Robot 2.0");

  //display battery voltage
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Battery - ");
  display.setCursor(55,8);
  display.print(v_sense);
  display.setCursor(80,8);
  display.print(" V");

  //display low voltage message
  if (v_sense < 11){
    cout = 50;                          //low battery command sent to the GCS
    display.setTextSize(1);
    display.setCursor(0,23);
    display.println(" Low Battery");
    while(1){
      Speaker();
      STOP();
      uv_on_flag= 0;
    }
  }

  //display lamp hour count
  display.setCursor(0,23);
  display.println("Lamp Hrs - ");
  display.setCursor(55,23);
  display.print(LAMP_HOUR);


  display.display();
  display.clearDisplay();
  //delay(1000);
}

void bat_voltage_read(){
  //measure battery voltage once in 60 seconds
  if ((millis() - t1) > 60000){
    for (int i = 0; i<10; i++){
      v_sense = v_sense + analogRead(bat_voltage_pin);
    }
  v_sense = v_sense /10;    
  v_sense = (v_sense/ 1365 )*11;
  v_sense = v_sense + 1.2;
  t1 = millis();
  }
}

void UV_control(){
  
  if ((uv_on_flag == 1 ) && (human_detected_flag == 0)){
    digitalWrite (uv_relay_pin,HIGH);
    UV_Buzzer();
  }
  else {
    digitalWrite (uv_relay_pin,LOW);
    }
}

void Speaker(){
    for (int i =0;i<5;i++) {
  tone(buzzer,740);
  delay(100);
  tone(buzzer,1047);
  delay(100);
  tone(buzzer,1319);
  delay(100);
  tone(buzzer,1568);
  delay(100);
  tone(buzzer,1319);
  delay(50);
  tone(buzzer,393);
  delay(50);
  tone(buzzer,740);
  delay(10);
  tone(buzzer,1047);
  delay(250);
  tone(buzzer,1319);
  delay(500);
  noTone(buzzer);
  delay(500);
  }
  noTone(buzzer);
}

void read_received_commands(){
  
  //system cutoff command
  if (c1 == 50){
    STOP();
    uv_on_flag = 0;
  }

  //GO FORWARD
  if (c1 == 100){
    //check for front obstacle
    if (obstacle_status[0] == 0){
      Forward();
    }
    else{
      STOP();
      cout = 10;    //obstacle detected front command sent to the GCS
    }
  }

  //Turn left
  if (c1 == 400){
    //check for left obstacle
    if (obstacle_status[1] == 0){
      Left();
    }
    else{
      STOP();
      cout = 20;    //obstacle detected left command sent to the GCS
    }

  }

  //Turn right
  if (c1 == 300){
    //check for right obstacle
    if (obstacle_status[3] == 0){
      Right();
    }
    else{
      STOP();
      cout = 30;    //obstacle detected right command sent to the GCS
    }

  }

  //Go backward
  if (c1 == 200){
        //check for rear obstacle
    if (obstacle_status[2] == 0){
      Backward();
    }
    else{
      STOP();
      cout = 40;    //obstacle detected rear command sent to the GCS
    }

  }

  //detect uv commands 
  if (c2 == 100){
    uv_on_flag= 1;
  }
  else {
    uv_on_flag =0;
  }
}

void Forward(){
  analogWrite(M2CC,MAX_ROBOT_SPEEDR);    //right
  analogWrite(M2CW,0);
  analogWrite(M1CW,0);                  //left
  analogWrite(M1CC,MAX_ROBOT_SPEEDL);
}
void Backward(){
  analogWrite(M2CC,0);
  analogWrite(M2CW,MAX_ROBOT_SPEEDR);
  analogWrite(M1CW,MAX_ROBOT_SPEEDL);
  analogWrite(M1CC,0);
}
void Left(){
  analogWrite(M2CW,MAX_ROBOT_SPEEDR);
  analogWrite(M2CC,0);
  analogWrite(M1CC,MAX_ROBOT_SPEEDL);
  analogWrite(M1CW,0);
}
void Right(){
  analogWrite(M2CW,0);
  analogWrite(M2CC,MAX_ROBOT_SPEEDR);
  analogWrite(M1CC,0);
  analogWrite(M1CW,MAX_ROBOT_SPEEDL);
}
void STOP(){
  analogWrite(M2CC,0);
  analogWrite(M2CW,0);
  analogWrite(M1CC,0);
  analogWrite(M1CW,0);
}
void nrf(){

  //read received commands
     if (radio.available()) {
        
        radio.read(&text, sizeof(text));
        c1 = text[0];
        c2 = text[1];
        radio.stopListening();

        radio.startListening();
    }

  //send notification messages
        textout[0] = cout;
        radio.write(&textout, sizeof(textout));  
        cout = 0 ;
}


void UV_Buzzer(){
  if (( millis() - t2) > 1000){
  tone(buzzer,740);
  t2 = millis();
  }
  if (( millis() - t3) > 2000){
  noTone(buzzer);
  t3 = millis();
  }
}

void pir_check(){
  if ((digitalRead(PIR1) == HIGH) || (digitalRead(PIR2) == HIGH) || (digitalRead(PIR3) == HIGH) || (digitalRead(PIR4) == HIGH) ) {
    human_detected_flag = 1;
    cout =  60;               //human detected command sent to the GCS
  }
  else {
    human_detected_flag = 0;
  }
}

void Cal_Lamp_Hours(){
  if (uv_on_flag== 1) {
    
    if ((millis()-t4) > 60000) {
      LAMP_MIN = LAMP_MIN + 1;
      t4 = millis();

      if (LAMP_MIN == 60){
        LAMP_HOUR = LAMP_HOUR + 1;
        LAMP_MIN  = 0;
      }

     //Write to EEPROM 
     writetoEEPROM(lamp_min_addr, LAMP_MIN);
     writetoEEPROM(lamp_hour_addr, LAMP_HOUR);
    }
    
  }
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

//Read eeprom values
int readFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

//Write values to eeprom
void writetoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}
