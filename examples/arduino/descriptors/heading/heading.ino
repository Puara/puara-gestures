// Puara Gestures - Heading (tilt-compensated compass) example
// Computes the compass heading (yaw) from accelerometer + magnetometer using
// the puara_gestures::Heading descriptor. Together with Tilt and Roll this
// gives a full 3D pointing direction.
//
// The heading is tilt-compensated, so it stays stable even when the device
// is not held flat. The magnetometer must be calibrated (hard/soft-iron)
// for the heading to be accurate.
//
// Two usage patterns are available:
//   - Tied mode   : the Heading reads its input from an external IMU struct automatically
//   - Direct mode : call update(accl, magn) with your readings each loop
//
// Open the Arduino Serial Plotter to see the heading as a live curve.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
puara_gestures::Imu9Axis imu;
puara_gestures::Heading heading{&imu};

// --- Direct mode (alternative, no tied struct) ---
// puara_gestures::Heading heading;
// Then call heading.update(accl, magn) in loop() instead of heading.update().

void setup() {
  Serial.begin(115200);

  heading.declination = 0.0;  // set your local magnetic declination (degrees)

  Serial.println("heading,roll,pitch");
}

void loop() {
  imu.accl = readAccel();  // replace with your accelerometer read
  imu.magn = readMag();    // replace with your (calibrated) magnetometer read
  heading.update();        // reads accl + magn from the tied imu

  // --- Direct mode equivalent ---
  // heading.update(readAccel(), readMag());

  Serial.print(heading.heading);  Serial.print(",");
  Serial.print(heading.roll);     Serial.print(",");
  Serial.println(heading.pitch);

  delay(20);  // ~50 Hz
}

// Placeholders: replace with your real sensor reads.
puara_gestures::Coord3D readAccel() { return {0.0, 0.0, 1.0}; }
puara_gestures::Coord3D readMag()   { return {1.0, 0.0, 0.0}; }
