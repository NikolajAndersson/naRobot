// ---------------------------------------------------------------------------
// This example code was used to successfully communicate with 15 ultrasonic sensors. You can adjust
// the number of sensors in your project by changing SONAR_NUM and the number of NewPing objects in the
// "sonar" array. You also need to change the pins for each sensor for the NewPing objects. Each sensor
// is pinged at 33ms intervals. So, one cycle of all sensors takes 495ms (33 * 15 = 495ms). The results
// are sent to the "oneSensorCycle" function which currently just displays the distance data. Your project
// would normally process the sensor results in this function (for example, decide if a robot needs to
// turn and call the turn function). Keep in mind this example is event-driven. Your complete sketch needs
// to be written so there's no "delay" commands and the loop() cycles at faster than a 33ms rate. If other
// processes take longer than 33ms, you'll need to increase PING_INTERVAL so it doesn't get behind.
// ---------------------------------------------------------------------------
#include <NewPing.h>
#include <Servo.h>
#include <NAservo.h>

#define SONAR_NUM     3 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

NAservo servo[3];

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

unsigned long time = 0;

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(2, 3, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(4, 5, MAX_DISTANCE),
  NewPing(6, 7, MAX_DISTANCE)
};

void setup() {
  Serial.begin(115200);

  servo[0].Setup(9, 0, 180);
  servo[1].Setup(10, 0, 180);
  servo[2].Setup(11, 0, 180);


  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;

  servo[0].MoveTo(50);
  servo[1].MoveTo(0);
  servo[2].MoveTo(142);
}

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) sendData(cm[0], cm[1], cm[2]); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // Other code that *DOESN'T* analyze ping results can go here.
  lookOut();
}

void wait(int milisec) {
  time = millis();
}


void lookOut() {
  time = millis();

  if (millis() < time + 1000) {

    int s0 = random() % 180;
    while (!servo[0].MoveTo(s0, 100)) {}
    int lean = random() % 30;
    while (!servo[1].MoveTo(90 - lean, 50) &&
           !servo[2].MoveTo(140 - lean, 50)) {}
    //delay(1000);
  }
  else if (time < millis() + 1700) {
    int tilt = (random() % 90) + 45;
    while (!servo[3].MoveTo(tilt, 20)) {}
    //delay(700);
  }
  else
    while (!servo[3].MoveTo(90, 200)) {}
}

void sendData(int s1, int s2, int s3) {
  byte buf[5] = {255, s1, s2, s3, 0};
  buf[4] = calculateCHKSum(buf, 5);
  /*for(int i = 0; i < sizeof(buf); i++){
  Serial.print(i);
  Serial.print(":   ");
  Serial.print(buf[i]);
  Serial.print(" ");
  }
  Serial.println();*/
  Serial.write(buf, 5);

}

byte calculateCHKSum(byte buf[], int len) {
  byte chk = 0;
  for (int i = 1; i < len - 1; i++) {
    chk += buf[i];
  }
  return chk;
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}


void DebugSensors() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}
