// Puara Gestures - Spin example
// Turns a gyroscope rate into rotation features (speed, accumulated angle,
// full-turn count, direction) using the puara_gestures::Spin descriptor.
//
// Unlike Tilt/Roll (which report a static pose), Spin reports motion about
// an axis - useful for twist/crank/knob-style control or counting turns.
//
// Two usage patterns are available:
//   - Tied mode   : the Spin reads its input from an external variable automatically
//   - Direct mode : call update(rate) with your input value each loop
//
// Open the Arduino Serial Plotter to see the outputs as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
double gyro_z = 0.0;  // degrees/second about the vertical axis
puara_gestures::Spin spin{&gyro_z};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Spin spin;
// Then call spin.update(rate) in loop() instead of spin.update().

void setup() {
  Serial.begin(115200);

  spin.revolution = 360.0;  // input units per full turn (360 for deg/s)
  spin.deadzone   = 1.0;    // ignore small rates as noise

  Serial.println("velocity,angle,revolutions,turns,direction");
}

void loop() {
  gyro_z = readGyroZ();  // replace with your gyroscope Z reading (deg/s)
  spin.update();         // integrates using the monotonic clock

  // --- Direct mode equivalent ---
  // spin.update(readGyroZ());

  Serial.print(spin.velocity);     Serial.print(",");
  Serial.print(spin.angle);        Serial.print(",");
  Serial.print(spin.revolutions);  Serial.print(",");
  Serial.print(spin.turns);        Serial.print(",");
  Serial.println(spin.direction);

  delay(10);  // ~100 Hz
}

// Placeholder: replace with your real gyroscope read.
double readGyroZ() {
  return 0.0;
}
