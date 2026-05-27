// Puara Gestures - LeakyIntegrator example
// Each new reading is added to a fraction of the previous output, so the
// signal accumulates then slowly decays when the input drops to zero.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  // leak=0.5: each step the accumulated value is multiplied by 0.5
  puara_gestures::utils::LeakyIntegrator integrator(0.0, 0.0, 0.5, 0, 0);

  double inputs[] = {10.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0};
  Serial.println("LeakyIntegrator (leak=0.5):");
  for (double v : inputs) {
    double out = integrator.integrate(v);
    Serial.print("  input=");
    Serial.print(v, 1);
    Serial.print(" -> output=");
    Serial.println(out, 3);
  }
}

void loop() {}
