// Puara Gestures - Smooth example
// Rolling average smoother: keeps the last N values and returns their mean.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  // Average over the last 3 readings
  puara_gestures::utils::Smooth smoother(3);

  double noisy[] = {1.0, 5.0, 3.0, 7.0, 2.0, 6.0, 4.0};
  Serial.println("Smooth (window=3):");
  for (double v : noisy) {
    double out = smoother.smooth(v);
    Serial.print("  raw=");
    Serial.print(v, 1);
    Serial.print(" -> smoothed=");
    Serial.println(out, 4);
  }

  smoother.clear();
  Serial.println("After clear(), first value:");
  Serial.println(smoother.smooth(10.0), 4);
}

void loop() {}
