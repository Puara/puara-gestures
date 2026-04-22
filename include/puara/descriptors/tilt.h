//********************************************************************************//
// Puara Gestures - Tilt (.h)                                                     //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
// Maggie Needham (2024)                                                          //
//********************************************************************************//

#pragma once

#include "IMU_Sensor_Fusion/imu_orientation.h"
#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{

/**
 * @file tilt.h
 * @brief Tilt estimator using accelerometer, gyroscope, and magnetometer data.
 *
 * This class wraps the IMU_Sensor_Fusion orientation filter and returns a
 * filtered tilt value in radians.
 *
 * Example:
 * @code
 * #include <puara/descriptors/tilt.h>
 *
 * puara_gestures::Coord3D accel{0.0, 0.0, 1.0};
 * puara_gestures::Coord3D gyro{0.0, 0.0, 0.0};
 * puara_gestures::Coord3D mag{0.3, 0.0, 0.5};
 * puara_gestures::Tilt tilt;
 *
 * void setup() {
 *   // sensor and serial initialization
 * }
 *
 * void loop() {
 *   // Read your IMU sensors into accel, gyro, mag
 *   double period = 0.01; // seconds since last sample
 *   double tiltValue = tilt.tilt(accel, gyro, mag, period);
 *   Serial.print("tilt=");
 *   Serial.println(tiltValue);
 * }
 * @endcode
 */
class Tilt : public utils::Smooth
{
public:
  IMU_Orientation orientation;

  using utils::Smooth::Smooth;

  /**
   * @brief Calculates tilt (aka "pitch") measurement
   *
   * @param accel Measured in G's
   * @param gyro Measured in degrees/sec
   * @param mag Measured in Gauss
   * @param period_sec Measured in seconds
   *
   * @return Output range of [- PI /2, PI /2]
   */
  double tilt(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec)
  {
    orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
    orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period_sec);
    orientation.setMagnetometerValues(mag.x, mag.y, mag.z);
    orientation.update(0.01);
    return orientation.euler.tilt;
  }

  /**
   * @brief Clears list of all previous inputs
   */
  void clear_smooth() { clear(); }
};
}
