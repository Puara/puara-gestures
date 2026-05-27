// Puara Gestures - KalmanQuaternionFilter example
// Shows how to fuse accelerometer, gyroscope, and magnetometer data
// into roll / pitch / yaw angles using a Kalman-style filter.
//
// In a real project, replace the simulated sensor values below with
// readings from your actual IMU sensor.
//
// Open the Arduino Serial Plotter to see roll, pitch, and yaw as live curves.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// Create the filter: first argument = process noise, second = measurement noise
puara_gestures::KalmanQuaternionFilter filter(0.001, 0.01);
puara_gestures::Imu9Axis imu;

void setup() {
  Serial.begin(115200);

  Serial.println("roll,pitch,yaw");  // CSV header for Serial Plotter
}

void loop() {
  float t = millis() / 1000.0;  // elapsed time in seconds

  // Simulate a device rocking back and forth
  // Replace these three lines with your real IMU sensor readings
  imu.gyro = {sin(t) * 0.5, sin(t * 0.7) * 0.3, 0.0};  // angular velocity (rad/s)
  imu.accl = {sin(t) * 2.0, sin(t * 0.7) * 1.5, 9.81};  // accelerometer (m/s²)
  imu.magn = {0.3, 0.0, 0.5};                            // magnetometer (static)

  // Update the filter and print the resulting orientation
  if (filter.update(imu, false)) {
    double roll, pitch, yaw;
    filter.getEulerDegrees(roll, pitch, yaw);

    Serial.print(roll);   Serial.print(",");
    Serial.print(pitch);  Serial.print(",");
    Serial.println(yaw);
  }

  delay(50);  // ~20 Hz update rate
}
