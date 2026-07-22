// Puara Gestures - PeakDetector example
// Flags local maxima (peaks) and minima (troughs) in a streaming signal
// using the puara_gestures::utils::PeakDetector helper.
//
// It reports the instant the signal turns around, so a continuous control
// becomes events - a gesture reaching its extent, a bounce, an oscillation
// cycle - without buffering the whole signal. The `delta` guard ignores
// wiggles smaller than the given swing.
//
// Open the Arduino Serial Plotter to see the signal and peak/trough markers.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define SENSOR_PIN A0

puara_gestures::utils::PeakDetector detector{5.0};  // ignore swings < 5 counts

void setup() {
  Serial.begin(115200);
  Serial.println("value,peak,trough");
}

void loop() {
  double value = analogRead(SENSOR_PIN);
  detector.update(value);

  if (detector.peak) {
    // e.g. trigger on the peak: detector.peakValue
  }
  if (detector.trough) {
    // e.g. trigger on the trough: detector.troughValue
  }

  Serial.print(value);                                Serial.print(",");
  Serial.print(detector.peak ? detector.peakValue : 0);   Serial.print(",");
  Serial.println(detector.trough ? detector.troughValue : 0);

  delay(10);  // ~100 Hz
}
