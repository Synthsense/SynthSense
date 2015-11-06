#include <NewPing.h>

// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------



#define TRIGGER_PIN_1  11  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_1     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN_2  9
#define ECHO_PIN_2     8
#define MOTOR_1        14
#define IR_IN          15
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar_1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar_2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


int baseline_IR;
int threshold_IR;
float curr_IR;

unsigned int threshold_us1;
unsigned int threshold_us2;
unsigned int curr_us1;
unsigned int curr_us2;
unsigned int curr_dist;
unsigned int total;

enum state_type {
  CALIBRATE,
  US_1,
  VIBRATE_1,
  US_2,
  VIBRATE_2,
  IR,
  VIBRATE_3,
};

state_type state;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  state = US_1;  // By right this should be CALIBRATION
  baseline_IR = 0;
  threshold_us1 = 100; // This is in centimeters!
  threshold_us2 = 100; // This is in centimeters!  
  pinMode(MOTOR_1, OUTPUT);
  total = 0;
}

void loop() {
  switch(state) {

    case CALIBRATE:
      // Collect 100 data points and average. Send buzz feedback to us_1
      state = US_1;  // By right this should be IR
      // Collect 100 data points
      for (int i = 0; i < 100; i++) {
          total += 1; // Change this
      }
      baseline_IR = total / 100; // Verify if this works
      break;

    case IR:
      curr_IR = analogRead(IR_IN) * ( 5.0 / 1023.0 ); // Change this
      Serial.print("IR:");
      Serial.println(curr_IR);
      state = US_1;
      break;

    case US_1:
      delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
      curr_us1 = sonar_1.ping(); // Send ping, get ping time in microseconds (uS).
      curr_dist = curr_us1  / US_ROUNDTRIP_CM;
      Serial.print("US_1: ");
      Serial.print(curr_dist); // Convert ping time to distance in cm and print result (0 = outside set distance range)
      Serial.println("cm");
      if ((curr_dist > threshold_us1) || (curr_dist <= 1.0)) {
        state = US_2;
      } else {
        Serial.println("OBSTACLE DETECTED!");
        state = VIBRATE_1;
      }
      break;

    case VIBRATE_1:
      // Vibrate lol;
      digitalWrite(MOTOR_1, HIGH);
      delay(500);
      digitalWrite(MOTOR_1, LOW);
      state = IR;
      break;

    case US_2:
      state = IR;
      break;

    default:
      Serial.print("DEFAULT");
      break;
  }
  
}
