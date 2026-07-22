// Puara Gestures - Ribbon example
// Turns a 1D touch strip (ribbon / SoftPot / linear position sensor) into
// playable gestures - position, slide velocity/direction, taps - using the
// puara_gestures::Ribbon descriptor.
//
// Feed it the current position and whether the strip is touched. Velocity is
// not computed across a lift-and-retouch, so moving your finger to a new spot
// does not produce a spurious jump.
//
// Open the Arduino Serial Plotter to see position and velocity as curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

#define RIBBON_PIN A0

puara_gestures::Ribbon ribbon;

void setup() {
  Serial.begin(115200);

  ribbon.tapMaxTime   = 200.0;  // ms; touches shorter than this can be taps
  ribbon.tapMaxTravel = 0.05;   // position units; taps barely move

  Serial.println("position,velocity,direction,touched,tap");
}

void loop() {
  int raw = analogRead(RIBBON_PIN);
  bool touched = raw > 8;              // most strips float low when untouched
  double pos = raw / 1023.0;           // normalize 0..1

  ribbon.update(pos, touched);

  if (ribbon.onset)   { /* finger down */ }
  if (ribbon.tap)     { /* quick tap    */ }
  if (ribbon.release) { /* finger up    */ }

  Serial.print(ribbon.position);   Serial.print(",");
  Serial.print(ribbon.velocity);   Serial.print(",");
  Serial.print(ribbon.direction);  Serial.print(",");
  Serial.print(ribbon.touched);    Serial.print(",");
  Serial.println(ribbon.tap);

  delay(10);  // ~100 Hz
}
