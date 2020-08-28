/* This code measures the distance from 8 sonar sensors and send the measurement 
 *  values to STM32.
 *  
 * Following is the sensor arrangement (top view) and measurement values are stored in the  
 * array according to that order. 
 * 
 *            1      2
 *        ---------------
 *       |      Front    |
 *     8 |               | 3
 *       |               | 
 *       |               | 
 *     7 |               | 4     
 *       |      Back     |
 *        ---------------
 *            6      5 
 */          


#include <NewPing.h>
#include <Wire.h>
#define SONAR_NUM     8 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define MX_OBS_DIS 100 
int x;
unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
    NewPing(A0, A1, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
    NewPing(A2, A3, MAX_DISTANCE),
    NewPing(2, 3, MAX_DISTANCE),
    NewPing(4, 5, MAX_DISTANCE),
    NewPing(6, 7, MAX_DISTANCE),
    NewPing(8, 9, MAX_DISTANCE),
    NewPing(10, 11, MAX_DISTANCE),
    NewPing(12, 13, MAX_DISTANCE),

};

void setup() {
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    
  Wire.begin(); 
}

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // Other code that *DOESN'T* analyze ping results can go here.
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(x);              // sends x 
  Wire.endTransmission(); 
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
    if ((cm[0] < MX_OBS_DIS) || (cm[1] < MX_OBS_DIS)){
      x=1;    //front
    }
    if ((cm[2] < MX_OBS_DIS) || (cm[3] < MX_OBS_DIS)){
      x=2;    //left
    }
    if ((cm[4] < MX_OBS_DIS) || (cm[5] < MX_OBS_DIS)){
      x=3;    //Right
    }
    if ((cm[6] < MX_OBS_DIS) || (cm[7] < MX_OBS_DIS)){
      x=4;    //Back
    }
    
  }
  if ((cm[0] > MX_OBS_DIS) && (cm[1] > MX_OBS_DIS) &&(cm[2] > MX_OBS_DIS) &&(cm[3] > MX_OBS_DIS)&&(cm[4] > MX_OBS_DIS)&&(cm[5] > MX_OBS_DIS)&&(cm[6] > MX_OBS_DIS)&&(cm[7] > MX_OBS_DIS) ){
    x=0;
  }
  Serial.println();
}
