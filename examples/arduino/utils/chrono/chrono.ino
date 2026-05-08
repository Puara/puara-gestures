// Puara Gestures - chrono example
// Demonstrates the portable high-resolution timer helpers.

#include <Arduino.h>
#include <puara/utils/chrono.h>

void setup() {
  Serial.begin(115200);

  unsigned long long t0 = puara_gestures::utils::getCurrentTimeMicroseconds();
  delay(100);
  unsigned long long t1 = puara_gestures::utils::getCurrentTimeMicroseconds();

  Serial.print("Elapsed microseconds (~100 ms): ");
  Serial.println((unsigned long)(t1 - t0));

  unsigned long long ms0 = puara_gestures::utils::getCurrentTimeMilliseconds();
  delay(50);
  unsigned long long ms1 = puara_gestures::utils::getCurrentTimeMilliseconds();

  Serial.print("Elapsed milliseconds (~50 ms):  ");
  Serial.println((unsigned long)(ms1 - ms0));
}

void loop() {}
