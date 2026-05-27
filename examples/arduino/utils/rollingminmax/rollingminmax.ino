// Puara Gestures - RollingMinMax example
// Tracks the minimum and maximum over the last N values using a sliding window.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  // Keep the min/max of the last 4 samples
  puara_gestures::utils::RollingMinMax<double> window(4);

  double samples[] = {3.0, 7.0, 1.0, 5.0, 9.0, 2.0, 4.0};
  Serial.println("RollingMinMax (window=4):");
  for (double s : samples) {
    auto range = window.update(s);
    Serial.print("  added ");
    Serial.print(s, 1);
    Serial.print(" -> min=");
    Serial.print(range.min, 1);
    Serial.print(", max=");
    Serial.println(range.max, 1);
  }
}

void loop() {}
