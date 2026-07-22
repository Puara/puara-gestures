// Puara Gestures - MultiTouch example
// Extracts relational two-finger gestures (pinch/spread scale, twist
// rotation, pan) from two touch positions using puara_gestures::MultiTouch.
//
// Feed it the coordinates of two contacts each frame and read scale,
// rotation, and the midpoint - the classic trackpad/phone gestures mapped
// to expressive continuous controls.
//
// Two usage patterns are available:
//   - Tied mode   : the MultiTouch reads the two points from external variables automatically
//   - Direct mode : call update(a, b) with your two points each loop
//
// Open the Arduino Serial Plotter to see scale and rotation as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
puara_gestures::Coord2D finger1{};
puara_gestures::Coord2D finger2{};
puara_gestures::MultiTouch touch{&finger1, &finger2};

// --- Direct mode (alternative, no tied variables) ---
// puara_gestures::MultiTouch touch;
// Then call touch.update(a, b) in loop() instead of touch.update().

bool wasTouching = false;

void setup() {
  Serial.begin(115200);
  Serial.println("distance,scale,rotation,center_x,center_y");
}

void loop() {
  bool touching = twoFingersDown();  // replace with your touch driver

  if (touching) {
    finger1 = readTouch(0);
    finger2 = readTouch(1);

    // Anchor the reference when the two-finger gesture first begins,
    // so scale starts at 1.0 and rotation at 0.
    if (!wasTouching) {
      touch.update();   // seed geometry
      touch.anchor();   // set reference to the starting pose
    } else {
      touch.update();   // reads finger1/finger2 automatically (tied mode)
    }

    Serial.print(touch.distance);   Serial.print(",");
    Serial.print(touch.scale);      Serial.print(",");
    Serial.print(touch.rotation);   Serial.print(",");
    Serial.print(touch.center.x);   Serial.print(",");
    Serial.println(touch.center.y);
  }
  wasTouching = touching;

  delay(10);  // ~100 Hz
}

// Placeholders: replace with your real touch driver.
bool twoFingersDown() { return true; }
puara_gestures::Coord2D readTouch(int) { return {0.0, 0.0}; }
