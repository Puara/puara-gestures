// Puara Gestures - MapRange example
// Remaps a value from one numeric range to another.

#include <Arduino.h>
#include <puara/utils/maprange.h>

void setup() {
  Serial.begin(115200);

  puara_gestures::utils::MapRange mapper;
  mapper.inMin  = 0;
  mapper.inMax  = 1023;
  mapper.outMin = 0.0;
  mapper.outMax = 1.0;

  int raw[] = {0, 256, 512, 768, 1023};
  Serial.println("MapRange (ADC 0-1023 -> 0.0-1.0):");
  for (int r : raw) {
    Serial.print("  ");
    Serial.print(r);
    Serial.print(" -> ");
    Serial.println(mapper.range(r), 4);
  }

  // Inverted output range
  mapper.outMin = 1.0;
  mapper.outMax = 0.0;
  Serial.println("MapRange (ADC 0-1023 -> 1.0-0.0, inverted):");
  for (int r : raw) {
    Serial.print("  ");
    Serial.print(r);
    Serial.print(" -> ");
    Serial.println(mapper.range(r), 4);
  }
}

void loop() {}
