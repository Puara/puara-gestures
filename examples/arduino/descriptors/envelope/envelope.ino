// Puara Gestures - Envelope follower example
// Tracks the amplitude/energy contour of a changing signal using the
// puara_gestures::Envelope descriptor.
//
// The envelope rises quickly toward louder input (attack) and falls back
// slowly when the input drops (release), like the meter on an audio
// compressor. Feed it any signal that varies over time and read a smooth
// loudness-style value to map to synthesis amplitude, brightness, etc.
//
// Two usage patterns are available:
//   - Tied mode   : the Envelope reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to compare the raw and enveloped curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

// --- Tied mode ---
double motion = 0.0;
puara_gestures::Envelope envelope{&motion};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Envelope envelope;
// Then call envelope.update(value) in loop() instead of envelope.update().

void setup() {
  Serial.begin(115200);

  // Coefficients are applied per update() call, in [0, 1].
  envelope.attack  = 0.6;   // rise fast toward louder input
  envelope.release = 0.05;  // fall slowly when the input drops
  envelope.rectify = true;  // follow |value| of a bipolar signal

  Serial.println("raw,envelope");
}

void loop() {
  // Center the reading around 0 so rectify tracks its magnitude.
  motion = analogRead(SENSOR_PIN) - 512.0;
  envelope.update();  // reads from motion automatically (tied mode)

  // --- Direct mode equivalent ---
  // envelope.update(analogRead(SENSOR_PIN) - 512.0);

  Serial.print(motion);                  Serial.print(",");
  Serial.println(envelope.current_value());

  delay(10);  // ~100 Hz
}
