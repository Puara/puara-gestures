/**
* @file tilt.h
* @brief Estimate tilt (pitch) from IMU data using a complementary filter.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <cmath>
#include "IMU_Sensor_Fusion/imu_orientation.h"
#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @class Tilt
 * @brief Tilt estimator using accelerometer, gyroscope, and magnetometer data.
 *
 * @details This class wraps the IMU_Sensor_Fusion orientation filter and returns a
 * filtered tilt value in radians.
 *
 * Example:
 * @code{.cpp}
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
 *
 * @ingroup puara_gestures_descriptors
 */
class Tilt : public utils::Smooth
{
public:
  IMU_Orientation orientation;

  using utils::Smooth::Smooth;

  /**
   * @brief Calculates tilt (aka "pitch") measurement.
   *
   * This method updates the internal IMU orientation filter using the
   * provided accelerometer, gyroscope, and magnetometer readings.
   *
   * @param accel Accelerometer vector in G's.
   * @param gyro Gyroscope vector in degrees per second.
   * @param mag Magnetometer vector in Gauss.
   * @param period_sec Time elapsed since the previous sample, in seconds.
   * @return Tilt value in radians, in the range [-PI/2, PI/2].
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
   * @brief Clear the internal smoothing history.
   *
   * This resets any previously accumulated smoothing state inherited from
   * `utils::Smooth`.
   */
  void clear_smooth() { clear(); }
};
}

