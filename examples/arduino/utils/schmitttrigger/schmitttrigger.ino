// Puara Gestures - Schmitt trigger (hysteresis gate) example
// Turns a noisy analog signal into a clean on/off state without chatter
// using the puara_gestures::utils::SchmittTrigger helper.
//
// A single threshold flickers when a noisy signal hovers around it. The
// Schmitt trigger switches ON only when the input rises to `high`, and OFF
// only when it falls to `low`; between the two it holds, so noise in that
// band cannot make it chatter.
//
// Open the Arduino Serial Plotter to see the raw signal and the gate state.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

// low = 300, high = 700 (raw ADC counts)
puara_gestures::utils::SchmittTrigger gate{300.0, 700.0};

void setup() {
  Serial.begin(115200);
  Serial.println("raw,state");
}

void loop() {
  double raw = analogRead(SENSOR_PIN);
  bool on = gate.update(raw);

  // rose / fell are true only on the loop where the gate switched.
  if (gate.rose) {
    // e.g. noteOn();
  }
  if (gate.fell) {
    // e.g. noteOff();
  }

  Serial.print(raw);             Serial.print(",");
  Serial.println(on ? 1000 : 0); // scaled so the state is visible on the plot

  delay(10);  // ~100 Hz
}
