// Puara Gestures - Segmenter example
// Splits a continuous activity signal into movement phrases, reporting when
// each gesture starts and ends, using the puara_gestures::Segmenter descriptor.
//
// Feed it an energy/activity signal (e.g. a Shake output or |gyro|). It uses
// hysteresis to stay robust to noise and reports onset/offset events plus the
// duration of each phrase.
//
// Two usage patterns are available:
//   - Tied mode   : the Segmenter reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to see the activity and phrase state.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
double energy = 0.0;
puara_gestures::Segmenter phrase{&energy};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Segmenter phrase;
// Then call phrase.update(value) in loop() instead of phrase.update().

void setup() {
  Serial.begin(115200);

  phrase.onThreshold      = 0.1;   // activity that starts a phrase
  phrase.offThreshold     = 0.05;  // activity that ends a phrase
  phrase.minActiveSamples = 2;     // debounce onset (updates)
  phrase.minSilentSamples = 3;     // debounce offset (updates)

  Serial.println("energy,active,onset,offset,duration,count");
}

void loop() {
  energy = readActivity();  // replace with your energy/activity source
  phrase.update();          // reads from energy automatically (tied mode)

  // --- Direct mode equivalent ---
  // phrase.update(readActivity());

  Serial.print(energy);            Serial.print(",");
  Serial.print(phrase.active);     Serial.print(",");
  Serial.print(phrase.onset);      Serial.print(",");
  Serial.print(phrase.offset);     Serial.print(",");
  Serial.print(phrase.duration);   Serial.print(",");
  Serial.println(phrase.count);

  delay(10);  // ~100 Hz
}

// Placeholder: replace with your real activity signal.
double readActivity() {
  return 0.0;
}
