void Oled (){
  v_sense = analogRead(PB1);
  v_sense = (v_sense/ 1365 )*11;
  v_sense = v_sense + 1.2;
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SRQ_Robotics");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(v_sense);
  display.setCursor(55,8);
  display.print(" V");
  if (v_sense < 11){
    display.setTextSize(1);
    display.setCursor(0,23);
    display.println("Low Battary");
  }
  display.display();
  display.clearDisplay();
  //delay(1000);
}

void RELAY(){
  if (Relay == 1 ){
    digitalWrite (PA8,HIGH);
  }
  else {
    digitalWrite (PA8,LOW);
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
void moter_drive(){
  if (c1 == 50){
    STOP();
  }
  if (c1 == 100){
    Forward();
  }
  if (c1 == 144){
    Left();
  }
  if (c1 == 44){
    Right();
  }
  if (c2 == 200){
    Relay = 1;
  }
  else {
    Relay =0;
  }
}
void Forward(){
  analogWrite(M2CC,MAX_ROBOT_SPEED);    //right
  analogWrite(M2CW,0);
  analogWrite(M1CC,0);                  //left
  analogWrite(M1CW,MAX_ROBOT_SPEED);
}
void Backward(){
  analogWrite(M2CC,0);
  analogWrite(M2CW,MAX_ROBOT_SPEED);
  analogWrite(M1CC,MAX_ROBOT_SPEED);
  analogWrite(M1CW,0);
}
void Left(){
  analogWrite(M2CC,MAX_ROBOT_SPEED);
  analogWrite(M2CW,0);
  analogWrite(M1CC,MAX_ROBOT_SPEED);
  analogWrite(M1CW,0);
}
void Right(){
  analogWrite(M2CC,0);
  analogWrite(M2CW,MAX_ROBOT_SPEED);
  analogWrite(M1CC,0);
  analogWrite(M1CW,MAX_ROBOT_SPEED);
}
void STOP(){
  analogWrite(M2CC,0);
  analogWrite(M2CW,0);
  analogWrite(M1CC,0);
  analogWrite(M1CW,0);
}
void NRF(){
     if (radio.available()) {
        
        radio.read(&text, sizeof(text));
            c1 = text[0];
        c2 = text[1];
        c3 = text[2];
        c4 = text[3]; 
}
}
  void disp(){          //Display the Data
      Serial.print(c1);
      Serial.print("\t");
      Serial.print(c2);
      Serial.print("\t");
      Serial.print(c3);
      Serial.print("\t");
      Serial.println(c4);
    }
