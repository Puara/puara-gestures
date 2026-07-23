// Puara Gestures - Breath / wind-controller example
// Turns a breath/pressure sensor reading into wind-instrument features
// (loudness envelope, blowing gate, note on/off, tonguing, blow/draw)
// using the puara_gestures::Breath descriptor.
//
// Two usage patterns are available:
//   - Tied mode   : the Breath reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to see the envelope and events as curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define PRESSURE_PIN A0

// --- Tied mode ---
double pressure_raw = 0.0;
puara_gestures::Breath breath{&pressure_raw};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Breath breath;
// Then call breath.update(value) in loop() instead of breath.update().

void setup() {
  Serial.begin(115200);

  breath.attack       = 0.7;   // envelope rise coefficient (0..1)
  breath.release      = 0.2;   // envelope fall coefficient (0..1)
  breath.onThreshold  = 0.1;   // envelope level that starts a note
  breath.offThreshold = 0.05;  // envelope level that ends a note
  breath.articulation = 0.15;  // sharp-rise threshold for tonguing

  Serial.println("pressure,active,onset,tongued,ended,polarity");
}

void loop() {
  // Center around 0 if your sensor is bipolar (blow positive, draw negative).
  pressure_raw = (analogRead(PRESSURE_PIN) - 512) / 512.0;
  breath.update();  // reads from pressure_raw automatically (tied mode)

  // --- Direct mode equivalent ---
  // breath.update((analogRead(PRESSURE_PIN) - 512) / 512.0);

  Serial.print(breath.pressure);   Serial.print(",");
  Serial.print(breath.active);     Serial.print(",");
  Serial.print(breath.onset);      Serial.print(",");
  Serial.print(breath.tongued);    Serial.print(",");
  Serial.print(breath.ended);      Serial.print(",");
  Serial.println(breath.polarity);

  delay(5);  // ~200 Hz to catch articulation
}
