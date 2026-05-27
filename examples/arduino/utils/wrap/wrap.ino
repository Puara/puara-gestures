// Puara Gestures - wrap (Unwrap / Wrap) example
//
// Unwrap: converts a sequence of periodic (wrapped) angles into a continuous
//         stream that never jumps by more than half the period.
//
// Wrap:   normalizes an arbitrary value back into [min, max).

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  // --- Unwrap ---
  // Input crosses the -PI/PI boundary; unwrap keeps the stream continuous.
  puara_gestures::utils::Unwrap unwrapper(-M_PI, M_PI);

  double wrappedAngles[] = {3.0, 3.1, -3.0, -2.9, -2.8};
  Serial.println("Unwrap (range -PI to PI):");
  for (double a : wrappedAngles) {
    double out = unwrapper.unwrap(a);
    Serial.print("  wrapped=");
    Serial.print(a, 3);
    Serial.print(" -> unwrapped=");
    Serial.println(out, 3);
  }

  // --- Wrap ---
  // Forces any value back into [0, 2*PI).
  puara_gestures::utils::Wrap wrapper(0.0, 2.0 * M_PI);

  double angles[] = {-0.5, 0.0, M_PI, 6.0, 7.5, -7.5};
  Serial.println("Wrap (range 0 to 2*PI):");
  for (double a : angles) {
    double out = wrapper.wrap(a);
    Serial.print("  input=");
    Serial.print(a, 3);
    Serial.print(" -> wrapped=");
    Serial.println(out, 3);
  }
}

void loop() {}
