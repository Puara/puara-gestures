// Puara Gestures - Freefall example
// Detects free-fall (near-weightless) moments and their duration from
// acceleration, using the puara_gestures::Freefall descriptor.
//
// A resting accelerometer reads ~1 g; when dropped or thrown it briefly
// becomes weightless and the magnitude falls toward 0. Freefall flags that
// window and times it - a playful trigger for throw-to-play or catch gestures.
//
// Two usage patterns are available:
//   - Tied mode   : the Freefall reads its input from an external Coord3D automatically
//   - Direct mode : call update(accel) with your acceleration each loop
//
// Open the Arduino Serial Plotter to see magnitude and free-fall state.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
puara_gestures::Coord3D accel{};
puara_gestures::Freefall freefall{&accel};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Freefall freefall;
// Then call freefall.update(accel) in loop() instead of freefall.update().

void setup() {
  Serial.begin(115200);

  freefall.threshold = 0.3;  // g; magnitude below this counts as free-fall

  Serial.println("magnitude,freefall,onset,landed,fallTime");
}

void loop() {
  accel = readAcceleration();  // replace with your accelerometer read, in g
  freefall.update();           // reads accel automatically (tied mode)

  // --- Direct mode equivalent ---
  // freefall.update(readAcceleration());

  Serial.print(freefall.magnitude);  Serial.print(",");
  Serial.print(freefall.freefall);   Serial.print(",");
  Serial.print(freefall.onset);      Serial.print(",");
  Serial.print(freefall.landed);     Serial.print(",");
  Serial.println(freefall.fallTime);

  delay(5);  // ~200 Hz to catch short drops
}

// Placeholder: replace with your real accelerometer read (in g).
puara_gestures::Coord3D readAcceleration() {
  return {0.0, 0.0, 1.0};  // resting: 1 g on z
}
