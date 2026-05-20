// Puara Gestures - Discretizer example
// Reports only when a value changes; ignores repeated identical readings.

#include <Arduino.h>
#include <puara/utils/discretizer.h>

void setup() {
  Serial.begin(115200);

  puara_gestures::utils::Discretizer<int> detector;

  constexpr int readings[] = {5, 5, 5, 7, 7, 3, 3, 3, 5};
  Serial.println("Discretizer (int):");
  for (int r : readings) {
    bool changed = detector.isNew(r);
    Serial.print("  reading=");
    Serial.print(r);
    Serial.print(" -> ");
    Serial.println(changed ? "NEW" : "same");
  }
}

void loop() {}
