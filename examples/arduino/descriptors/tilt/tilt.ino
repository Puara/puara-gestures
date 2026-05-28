// Puara Gestures - Tilt example
// Shows how to estimate tilt (pitch) from accelerometer, gyroscope, and
// magnetometer data using the puara_gestures::Tilt complementary filter.
//
// tilt() returns a value in radians in the range [-PI/2, PI/2].
//
// Important: sensor units differ from the AHRS filter examples —
//   - Accelerometer : G's           (1 G = 9.81 m/s² ; flat device = 1 G on Z)
//   - Gyroscope     : degrees/second (not radians)
//   - Magnetometer  : Gauss
//
// In a real project, replace the simulated values with readings from your IMU.
//
// Open the Arduino Serial Plotter to see tilt in radians and degrees.

#include <Arduino.h>
#include <puara-gestures.h>

puara_gestures::Tilt tilt;

puara_gestures::Coord3D accel;
puara_gestures::Coord3D gyro;
puara_gestures::Coord3D mag;

const float RAD_TO_DEG = 57.2958;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);

  // Initial values: device lying flat, no rotation, magnetometer pointing north.
  // Coord3D members default to 0.0, so only non-zero values need to be set.
  accel.z = 1.0;   // 1 G on Z = device flat (gravity pointing straight down)
  mag = {0.3, 0.0, 0.5};  // pointing roughly north (Gauss)

  // clear_smooth() resets any accumulated smoothing history in the filter.
  // Useful when restarting or reinitializing the detector mid-session.
  tilt.clear_smooth();

  lastTime = millis();

  Serial.println("tilt_rad,tilt_deg");  // CSV header for Serial Plotter
}

void loop() {
  unsigned long now    = millis();
  double period_sec    = (now - lastTime) / 1000.0;  // elapsed time since last update
  lastTime             = now;
  float t              = now / 1000.0;               // total elapsed time in seconds

  // Simulate a device rocking back and forth around the X axis.
  // Replace these with real IMU sensor readings in your project.
  float tiltAngle = sin(t * 0.8) * 0.5;             // oscillating tilt angle (radians)
  accel.x = sin(tiltAngle);                          // G's: gravity projection on X
  accel.z = cos(tiltAngle);                          // G's: gravity projection on Z
  gyro.x  = cos(t * 0.8) * 0.4 * RAD_TO_DEG;        // angular velocity (degrees/second)
  // mag stays static — magnetometer changes slowly in real use

  // Update the filter and get the tilt angle in radians
  double tiltValue = tilt.tilt(accel, gyro, mag, period_sec);

  // Print both radians and degrees for readability
  Serial.print(tiltValue);                  Serial.print(",");
  Serial.println(tiltValue * RAD_TO_DEG);

  delay(10);  // ~100 Hz update rate
}
