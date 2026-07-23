// Puara Gestures - Derivative example
// Computes velocity, acceleration and jerk from a continuous position signal
// using the puara_gestures::Derivative descriptor.
//
// Many sensors report a position-like value; the expressive information is
// often in how fast it changes. Derivatives amplify noise, so smooth the
// input first (e.g. with the OneEuro filter) for musical results.
//
// Two usage patterns are available:
//   - Tied mode   : the Derivative reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to see the outputs as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

// --- Tied mode ---
double position = 0.0;
puara_gestures::Derivative derivative{&position};

// Optional: smooth the raw input before differentiating.
puara_gestures::utils::OneEuro smoother{1.0, 0.01};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Derivative derivative;
// Then call derivative.update(value) in loop() instead of derivative.update().

void setup() {
  Serial.begin(115200);
  Serial.println("position,velocity,acceleration,jerk");
}

void loop() {
  position = smoother.filter(analogRead(SENSOR_PIN));
  derivative.update();  // reads from position automatically (tied mode)

  // --- Direct mode equivalent ---
  // derivative.update(smoother.filter(analogRead(SENSOR_PIN)));

  Serial.print(derivative.position);      Serial.print(",");
  Serial.print(derivative.velocity);      Serial.print(",");
  Serial.print(derivative.acceleration);  Serial.print(",");
  Serial.println(derivative.jerk);

  delay(10);  // ~100 Hz
}
