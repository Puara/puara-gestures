// Puara Gestures - Impact / strike detector example
// Turns spikes in a magnitude signal into one trigger per hit, with a
// velocity-like intensity, using the puara_gestures::Impact descriptor.
//
// Feed it a positive magnitude that spikes on a hit (accelerometer
// magnitude, a piezo reading, contact energy). Each spike fires `strike`
// once and reports the peak as `intensity` - like a drum pad turning a
// physical hit into a note-on with velocity.
//
// Two usage patterns are available:
//   - Tied mode   : the Impact reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to see strikes and intensity as curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define PIEZO_PIN A0

// --- Tied mode ---
double magnitude = 0.0;
puara_gestures::Impact impact{&magnitude};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Impact impact;
// Then call impact.update(value) in loop() instead of impact.update().

void setup() {
  Serial.begin(115200);

  impact.threshold        = 2.0;  // magnitude that fires a strike
  impact.releaseThreshold = 0.8;  // must fall below this to re-arm
  impact.minInterval      = 30;   // ms refractory (0 = hysteresis only)

  Serial.println("magnitude,strike,intensity");
}

void loop() {
  // Read a positive magnitude. For a bipolar sensor use its magnitude.
  magnitude = analogRead(PIEZO_PIN) / 100.0;
  impact.update();  // reads from magnitude automatically (tied mode)

  // --- Direct mode equivalent ---
  // impact.update(analogRead(PIEZO_PIN) / 100.0);

  // strike    : true for one loop when a new hit is detected
  // intensity : peak magnitude of the current/last hit (its velocity)
  Serial.print(magnitude);         Serial.print(",");
  Serial.print(impact.strike);     Serial.print(",");
  Serial.println(impact.intensity);

  delay(5);  // ~200 Hz to catch sharp transients
}
