// Puara Gestures - 1€ filter example
// Smooths a noisy analog input using the puara_gestures::utils::OneEuro filter.
//
// The 1€ filter removes jitter while keeping lag low: it filters hard when
// the signal is still and eases off when the signal moves fast. Tune it with
// two knobs:
//   - mincutoff : lower = smoother at rest but laggier (start ~1.0)
//   - beta      : higher = less lag on fast moves (start at 0.0 and raise)
//
// Open the Arduino Serial Plotter to compare the raw and filtered curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

// mincutoff = 1.0 Hz, beta = 0.007
puara_gestures::utils::OneEuro smoother{1.0, 0.007};

void setup() {
  Serial.begin(115200);
  Serial.println("raw,filtered");
}

void loop() {
  double raw = analogRead(SENSOR_PIN);

  // filter(value) times the step with the internal monotonic clock.
  // Use filter(value, dt) instead if you already know your loop period.
  double filtered = smoother.filter(raw);

  Serial.print(raw);      Serial.print(",");
  Serial.println(filtered);

  delay(10);  // ~100 Hz
}
