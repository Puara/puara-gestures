// Puara Gestures - Rhythm example
// Extracts rhythmic features (frequency, tempo, figure/subdivision, beat/bar)
// from a discrete input over time using the puara_gestures::Rhythm class.
//
// Feed it the same kind of signal you would feed a Button: a value that goes
// above `threshold` on each event (an "onset") and back below it afterwards.
// From the timing of successive onsets Rhythm derives the tempo and meter.
//
// Two usage patterns are available:
//   - Tied mode   : the Rhythm reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Wiring: connect a button between BUTTON_PIN and GND and tap a steady pulse.
// INPUT_PULLUP means the pin reads HIGH when released and LOW when pressed,
// so we invert the reading with (1 - digitalRead()) below.
//
// Open the Arduino Serial Plotter to see the outputs as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define BUTTON_PIN 2

// --- Tied mode ---
// tap_data is updated each loop; the Rhythm object reads it automatically.
int tap_data = 0;
puara_gestures::Rhythm rhythm{&tap_data};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Rhythm rhythm;
// Then call rhythm.update(value) in loop() instead of rhythm.update().

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Optional: tune the detector.
  rhythm.threshold    = 1;      // minimum value to count as an onset   (default: 1)
  rhythm.smoothing    = 0.5;    // beat-period smoothing, 0..1           (default: 0.5)
  rhythm.timeout      = 3000.0; // ms of silence that resets the pulse   (default: 3000)
  rhythm.timeSignatureNumerator   = 4;  // beats per bar
  rhythm.timeSignatureDenominator = 4;  // note value that gets the beat
  // rhythm.estimateTimeSignature = true; // experimental meter guessing

  Serial.println("frequency,tempo,figure,subdivision,beat,bar");
}

void loop() {
  // Read the hardware pin and store it in tap_data.
  // INPUT_PULLUP is active-low, so we invert: 1 = pressed, 0 = released.
  tap_data = 1 - digitalRead(BUTTON_PIN);
  rhythm.update();  // reads from tap_data automatically (tied mode)

  // --- Direct mode equivalent ---
  // rhythm.update(1 - digitalRead(BUTTON_PIN));

  // frequency   : instantaneous onset rate in Hz (1000 / last interval)
  // tempo       : beat tempo in BPM (60000 / estimated beat period)
  // figure      : last interval relative to the beat (1 = beat, 0.5 = eighth)
  // subdivision : equal parts the last interval split the beat into
  // beat        : current beat position inside the bar (0-based)
  // bar         : completed bar count
  Serial.print(rhythm.frequency);   Serial.print(",");
  Serial.print(rhythm.tempo);       Serial.print(",");
  Serial.print(rhythm.figure);      Serial.print(",");
  Serial.print(rhythm.subdivision); Serial.print(",");
  Serial.print(rhythm.beat);        Serial.print(",");
  Serial.println(rhythm.bar);

  delay(10);  // ~100 Hz polling rate
}
