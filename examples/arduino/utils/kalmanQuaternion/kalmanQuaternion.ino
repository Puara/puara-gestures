// Puara Gestures - KalmanQuaternionFilter example
// Fuses 9-DoF IMU data into an orientation quaternion using a simplified
// Kalman-style filter.  The first update() call primes the timer; subsequent
// calls in loop() produce roll/pitch/yaw estimates.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

static puara_gestures::KalmanQuaternionFilter filter(0.001, 0.01);
static puara_gestures::Imu9Axis imu;
static int updateCount = 0;

void setup() {
  Serial.begin(115200);

  // Static IMU sample: gravity on Z, small gyro values (rad/s), magnetometer
  imu.accl = {0.0, 0.0, 9.81};
  imu.gyro = {0.01, 0.02, 0.005};
  imu.magn = {0.3,  0.0,  0.5};

  // Prime the internal timestamp
  filter.update(imu, false);

  Serial.println("KalmanQuaternionFilter - sending updates every 50 ms:");
}

void loop() {
  if (updateCount < 10) {
    delay(50);
    bool ok = filter.update(imu, false);  // gyro already in rad/s
    if (ok) {
      double roll, pitch, yaw;
      filter.getEulerDegrees(roll, pitch, yaw);
      Serial.print("Update ");
      Serial.print(updateCount + 1);
      Serial.print(": roll=");
      Serial.print(roll, 3);
      Serial.print(" pitch=");
      Serial.print(pitch, 3);
      Serial.print(" yaw=");
      Serial.println(yaw, 3);
    }
    updateCount++;
  }
}
