// Puara Gestures - Tilt example
// Estimates tilt (pitch) from IMU data using a complementary filter.
// Replace the placeholder sensor reads below with your actual IMU library calls.
//
// tilt() returns a value in radians in the range [-PI/2, PI/2].
//
// Note: sensor units here differ from the AHRS filter examples (Kalman, Madgwick, Mahony) —
//   - Accelerometer : G's            (1 G = 9.81 m/s² ; flat device = ~1 G on Z)
//   - Gyroscope     : degrees/second (not radians)
//   - Magnetometer  : Gauss
//
// Open the Arduino Serial Plotter to see tilt in radians and degrees.

#include <Arduino.h>
#include <puara-gestures.h>

puara_gestures::Tilt tilt;
puara_gestures::Imu9Axis imu;

const float RAD_TO_DEG = 57.2958;
unsigned long lastTime  = 0;

void setup() {
  Serial.begin(115200);

  // Initialize your IMU sensor here
  // e.g. IMU.begin();

  // Optional: reset any accumulated smoothing history
  tilt.clear_smooth();

  lastTime = millis();
  Serial.println("tilt_rad,tilt_deg,accl_x,accl_y,accl_z,gyro_x,gyro_y,gyro_z,magn_x,magn_y,magn_z");
}

void loop() {
  unsigned long now = millis();
  double period_sec = (now - lastTime) / 1000.0;  // elapsed time since last update (seconds)
  lastTime = now;

  // Replace with your IMU sensor reads:
  imu.accl.x = 0.0;  // e.g. sensor.getAccelX()  — in G's
  imu.accl.y = 0.0;  // e.g. sensor.getAccelY()
  imu.accl.z = 0.0;  // e.g. sensor.getAccelZ()

  imu.gyro.x = 0.0;  // e.g. sensor.getGyroX()   — in degrees/second
  imu.gyro.y = 0.0;  // e.g. sensor.getGyroY()
  imu.gyro.z = 0.0;  // e.g. sensor.getGyroZ()

  imu.magn.x = 0.0;  // e.g. sensor.getMagX()    — in Gauss
  imu.magn.y = 0.0;  // e.g. sensor.getMagY()
  imu.magn.z = 0.0;  // e.g. sensor.getMagZ()

  double tiltValue = tilt.tilt(imu.accl, imu.gyro, imu.magn, period_sec);

  Serial.print(tiltValue);          Serial.print(",");
  Serial.print(tiltValue * RAD_TO_DEG); Serial.print(",");
  Serial.print(imu.accl.x);        Serial.print(",");
  Serial.print(imu.accl.y);        Serial.print(",");
  Serial.print(imu.accl.z);        Serial.print(",");
  Serial.print(imu.gyro.x);        Serial.print(",");
  Serial.print(imu.gyro.y);        Serial.print(",");
  Serial.print(imu.gyro.z);        Serial.print(",");
  Serial.print(imu.magn.x);        Serial.print(",");
  Serial.print(imu.magn.y);        Serial.print(",");
  Serial.println(imu.magn.z);

  delay(10);  // ~100 Hz update rate
}
