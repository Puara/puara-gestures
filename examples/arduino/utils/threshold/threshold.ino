// Puara Gestures - Threshold (ThresholdT) example
// Clamps a value to a configurable [min, max] range.
// `current` always holds the raw (unclamped) input.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  puara_gestures::utils::Threshold thresh(-1.0, 1.0);

  double inputs[] = {0.5, 1.2, -0.8, -2.5, 0.0, 1.0};
  Serial.println("Threshold (min=-1.0, max=1.0):");
  for (double v : inputs) {
    double out = thresh.update(v);
    Serial.print("  input=");
    Serial.print(v, 2);
    Serial.print(" -> clamped=");
    Serial.print(out, 2);
    Serial.print(", raw=");
    Serial.println(thresh.current, 2);
  }

  // Integer threshold
  puara_gestures::utils::ThresholdT<int> intThresh(0, 255);
  Serial.println("ThresholdT<int> (min=0, max=255):");
  int intInputs[] = {-10, 0, 128, 255, 300};
  for (int v : intInputs) {
    int out = intThresh.update(v);
    Serial.print("  input=");
    Serial.print(v);
    Serial.print(" -> clamped=");
    Serial.println(out);
  }
}

void loop() {}
