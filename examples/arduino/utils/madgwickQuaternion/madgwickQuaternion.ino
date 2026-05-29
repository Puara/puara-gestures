// Puara Gestures - MadgwickQuaternionFilter example
// Shows how to fuse accelerometer, gyroscope, and magnetometer data
// into orientation using the Madgwick AHRS algorithm.
//
// Demonstrates the two main output formats:
//   - getEulerDegrees() : roll / pitch / yaw in degrees (intuitive for most use cases)
//   - getQuaternion()   : raw quaternion (w, x, y, z) for 3D engines and further math
//
// In a real project, replace the simulated sensor values below with
// readings from your actual IMU sensor.
//
// Open the Arduino Serial Plotter to see all values as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// Create the filter with a fusion gain of 0.1 (default).
// Larger values correct drift faster; smaller values produce smoother output.
puara_gestures::MadgwickQuaternionFilter filter{0.1};
puara_gestures::Imu9Axis imu;

void setup() {
  Serial.begin(115200);
  Serial.println("roll,pitch,yaw,qw,qx,qy,qz");  // CSV header for Serial Plotter
}

void loop() {
  float t = millis() / 1000.0;  // elapsed time in seconds

  // Simulate a device rocking back and forth
  // Replace these three lines with your real IMU sensor readings
  imu.gyro = {sin(t) * 0.5, sin(t * 0.7) * 0.3, 0.0};  // angular velocity (rad/s)
  imu.accl = {sin(t) * 2.0, sin(t * 0.7) * 1.5, 9.81};  // accelerometer (m/s²)
  imu.magn = {0.3, 0.0, 0.5};                            // magnetometer (static)

  if (filter.update(imu, false)) {
    // Euler angles: intuitive roll/pitch/yaw in degrees
    double roll, pitch, yaw;
    filter.getEulerDegrees(roll, pitch, yaw);

    // Raw quaternion: use this for 3D math, game engines, or further processing
    auto q = filter.getQuaternion();

    Serial.print(roll);    Serial.print(",");
    Serial.print(pitch);   Serial.print(",");
    Serial.print(yaw);     Serial.print(",");
    Serial.print(q.w, 4);  Serial.print(",");
    Serial.print(q.x, 4);  Serial.print(",");
    Serial.print(q.y, 4);  Serial.print(",");
    Serial.println(q.z, 4);
  }

  delay(50);  // ~20 Hz update rate
}
