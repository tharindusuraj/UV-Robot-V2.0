
#include <NewPing.h>
#include <Wire.h>

#define SONAR_NUM     8 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define obs_distance 30  //Threshold to detect obstacles

//Represents obstacle status in front,left,rear and right respectively.
//0 for obstacle free. 1 for obstacle detected.
int obstacle_array[4] = {0,0,0,0};    

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.
unsigned int cm_filtered[SONAR_NUM];

// Each sensor's trigger pin, echo pin, and max distance to ping.
NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(A0, A1, MAX_DISTANCE), //
  NewPing(A2, A3, MAX_DISTANCE), //Front two sensors
  NewPing(2, 3, MAX_DISTANCE),
  NewPing(4, 5, MAX_DISTANCE),   //Left two sensors 
  NewPing(6, 7, MAX_DISTANCE),
  NewPing(8, 9, MAX_DISTANCE),    //Rear two sensors
  NewPing(10, 11, MAX_DISTANCE),
  NewPing(12, 13, MAX_DISTANCE),  // Right two sensors

};

void setup() {

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
  // write obstacle data to I2C 
  Wire.beginTransmission(4); // transmit to device #4
  for (int i=0; i<5 ; i++){
    Wire.write(obstacle_array[i]); 
  }
  Wire.endTransmission();    // stop transmitting

}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // Detect obstacles based on the distance

  if ((cm[0] != 0 && cm[0]<obs_distance) or  (cm[1] != 0 && cm[1]<obs_distance)) obstacle_array[0] = 1;
  else obstacle_array[0] = 0;

  if ((cm[2] != 0 && cm[2]<obs_distance) or  (cm[3] != 0 && cm[3]<obs_distance)) obstacle_array[1] = 1;
  else obstacle_array[1] = 0;

  if ((cm[4] != 0 && cm[4]<obs_distance) or  (cm[5] != 0 && cm[5]<obs_distance)) obstacle_array[2] = 1;
  else obstacle_array[2] = 0;

  if ((cm[6] != 0 && cm[6]<obs_distance) or  (cm[7] != 0 && cm[7]<obs_distance)) obstacle_array[3] = 1;
  else obstacle_array[3] = 0;

}
