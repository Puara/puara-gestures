// Puara Gestures - Normalizer example
// Rescales a raw sensor to 0..1 using the min/max it has actually seen,
// with no manual calibration, via puara_gestures::utils::Normalizer.
//
// Wave the sensor through its full range once and the output will span
// 0..1. An optional decay slowly relaxes the learned range so it keeps
// adapting; set calibrating = false to freeze it once you are happy.
//
// Open the Arduino Serial Plotter to compare the raw and normalized curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

puara_gestures::utils::Normalizer norm;  // outputs 0..1

void setup() {
  Serial.begin(115200);

  // norm.decay = 0.001;        // slowly forget stale extremes (0 = expand-only)
  // norm.calibrating = false;  // freeze the learned range

  Serial.println("raw,normalized");
}

void loop() {
  double raw = analogRead(SENSOR_PIN);
  double normalized = norm.update(raw);

  Serial.print(raw);              Serial.print(",");
  Serial.println(normalized, 4);

  delay(10);  // ~100 Hz
}
