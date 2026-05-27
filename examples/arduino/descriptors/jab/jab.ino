// Puara Gestures - Jab example
// Shows how to detect directional impulse movements from accelerometer data
// using Jab (1D), Jab2D, and Jab3D detectors.
//
// Each detector watches a rolling window of sensor values and reports a score
// when a sudden directional change (max - min over last 10 samples) exceeds
// a threshold.
//
// Three detectors are demonstrated side by side:
//   - Jab   : single axis
//   - Jab2D : two axes (X and Y)
//   - Jab3D : three axes (X, Y, Z)
//
// Two usage patterns are shown across the examples:
//   - Tied mode   : detector reads from an external variable automatically
//   - Direct mode : pass the value to update() explicitly each loop
//
// In a real project, replace the simulated values with readings from your IMU.
//
// Open the Arduino Serial Plotter to see all jab scores as live curves.

#include <Arduino.h>
#include <puara-gestures.h>

// --- Jab 1D (tied mode) ---
// Watches a single acceleration axis.
// The detector reads from accel_x automatically each update().
double accel_x = 0.0;
puara_gestures::Jab jab(&accel_x);

// --- Jab2D (direct mode) ---
// Watches X and Y axes independently.
// Values are passed directly to update() instead of using a tied variable.
puara_gestures::Jab2D jab2d;

// --- Jab3D (tied mode with Coord3D) ---
// Watches all three axes. Tied to a Coord3D struct.
puara_gestures::Coord3D accel3d;
puara_gestures::Jab3D jab3d(&accel3d);

void setup() {
  Serial.begin(115200);

  // Set the detection threshold for each detector.
  // A jab fires when (max - min) over the last 10 samples exceeds this value.
  // Note: Jab (1D) exposes threshold as a public member,
  //       Jab2D and Jab3D expose it as a method that sets all axes at once.
  jab.threshold = 5;   // 1D: direct member access
  jab2d.threshold(5);  // 2D: sets x and y thresholds together
  jab3d.threshold(5);  // 3D: sets x, y, and z thresholds together

  Serial.println("jab1d,jab2d_x,jab2d_y,jab3d_x,jab3d_y,jab3d_z");
}

void loop() {
  float t = millis() / 1000.0;  // elapsed time in seconds

  // Simulate oscillating accelerometer motion on each axis.
  // Different frequencies produce independent jab events per axis.
  // Replace these with real IMU readings in your project.
  accel_x   = sin(t * 2.0) * 8.0;  // 1D: single axis
  accel3d.x = sin(t * 2.0) * 8.0;  // 3D: X axis (same motion as 1D for comparison)
  accel3d.y = sin(t * 1.3) * 6.0;  // 3D: Y axis (different frequency)
  accel3d.z = sin(t * 0.9) * 4.0;  // 3D: Z axis (slower, lower amplitude)

  // Update each detector
  jab.update();                                      // tied mode: reads accel_x
  jab2d.update(accel3d.x, accel3d.y);               // direct mode: values passed explicitly
  jab3d.update();                                    // tied mode: reads accel3d

  // Read the jab scores
  double                   score1d = jab.current_value();
  puara_gestures::Coord2D  score2d = jab2d.current_value();
  puara_gestures::Coord3D  score3d = jab3d.current_value();

  // Print all scores as CSV for Serial Plotter
  Serial.print(score1d);    Serial.print(",");
  Serial.print(score2d.x);  Serial.print(",");
  Serial.print(score2d.y);  Serial.print(",");
  Serial.print(score3d.x);  Serial.print(",");
  Serial.print(score3d.y);  Serial.print(",");
  Serial.println(score3d.z);

  delay(10);  // ~100 Hz update rate
}
