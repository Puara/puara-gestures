/**
 * @file simple_tilt_roll.h
 * @brief Lightweight 3DoF tilt and roll extractor for IMUs without a magnetometer.
 * @see https://github.com/Puara/puara-gestures
 * @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
 * @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
 * @author Edu Meneses (2025) - https://www.edumeneses.com
 */
#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @class three_dof_tilt_roll
 * @brief Lightweight 3DoF tilt and roll extractor for IMUs without a magnetometer.
 *
 * @details This class computes tilt and roll from accelerometer data using a simple
 * trig-based approximation. It does not perform full attitude estimation or
 * magnetic heading correction, and it assumes the accelerometer is dominated by
 * gravity. In dynamic motion or when linear acceleration is present, the
 * result may be less precise than a full sensor-fusion filter.
 *
 * It can optionally use a tied `Coord3D` pointer so the caller may update raw
 * IMU data externally and then call `update()` without parameters.
 *
 * Example:
 * @code{.cpp}
 * #include <puara/descriptors/simple_tilt_roll.h>
 *
 * puara_gestures::Coord3D imu_data{0.0, 0.0, 0.0};
 * puara_gestures::three_dof_tilt_roll tiltRoll(&imu_data);
 *
 * void setup() {
 *   // Initialize sensors and serial output here.
 * }
 *
 * void loop() {
 *   // Fill imu_data from your accelerometer and gyroscope.
 *   imu_data.x = readAccelX();
 *   imu_data.y = readAccelY();
 *   imu_data.z = readAccelZ();
 *
 *   tiltRoll.update();
 *
 *   double roll = tiltRoll.current_roll_value();
 *   double tilt = tiltRoll.current_tilt_value();
 *
 *   Serial.print("roll=");
 *   Serial.print(roll);
 *   Serial.print(" tilt=");
 *   Serial.println(tilt);
 * }
 * @endcode
 */
class Tilt_Roll
{
public:
  Tilt_Roll() noexcept
      : tied_x(nullptr)
      , tied_y(nullptr)
      , tied_z(nullptr)
  {
  }

  Tilt_Roll(const Tilt_Roll&) noexcept = default;
  Tilt_Roll(Tilt_Roll&&) noexcept = default;
  Tilt_Roll& operator=(const Tilt_Roll&) noexcept = default;
  Tilt_Roll& operator=(Tilt_Roll&&) noexcept = default;

  /**
   * @brief Construct a tilt/roll extractor tied to an external IMU sample.
   * @param tied Pointer to a `Coord3D` containing accelerometer data.
   */
  explicit Tilt_Roll(Coord3D* tied)
    : tied_x(&(tied->x))
    , tied_y(&(tied->y))
    , tied_z(&(tied->z))
  {
  }

  /**
   * @brief Update tilt and roll using raw accelerometer axes.
   *
   * @param accelx Accelerometer X axis value.
   * @param accely Accelerometer Y axis value.
   * @param accelz Accelerometer Z axis value.
   * @return 1 when the update is processed.
   */
  int update(double accelx, double accely, double accelz)
  {
    // calculate polar representation of accelerometer data
    roll = atan2(accelz, accely);
    magnitude = sqrt(pow(accelz, 2) + pow(accely, 2));
    tilt = atan2(accelx, magnitude);
    magnitude = sqrt(pow(accelx, 2) + pow(magnitude, 2));
    magnitude *= 0.00390625;

    return 1;
  }

  /**
   * @brief Update tilt and roll from a `Coord3D` accelerometer sample.
   * @param imu_data Accelerometer sample containing X, Y, and Z values.
   * @return 1 when the update is processed.
   */
  int update(Coord3D imu_data)
  {
    update(imu_data.x, imu_data.y, imu_data.z);
    return 1;
  }

  /**
   * @brief Update tilt and roll from the tied external IMU sample.
   *
   * This overload reads the current accelerometer values from the tied
   * `Coord3D` pointers and computes the current orientation.
   * @return 1 when the update is processed; 0 if no tied data is available.
   */
  int update()
  {
    if(tied_x != nullptr || tied_y != nullptr || tied_z != nullptr)
    {
      update(*tied_x, *tied_y, *tied_z);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current tilt/roll orientation values.
   * @return A `Simple_Orientation` containing roll, tilt, and magnitude.
   */
  Simple_Orientation current_value() const
  {
    return Simple_Orientation{roll, tilt, magnitude};
  }

  /**
   * @brief Get the current roll value.
   * @return Roll measurement in radians.
   */
  double current_roll_value() const
  {
    return roll;
  }

  /**
   * @brief Get the current tilt value.
   * @return Tilt measurement in radians.
   */
  double current_tilt_value() const
  {
    return tilt;
  }

  /**
   * @brief Tie this object to external accelerometer data.
   *
   * After calling this, `update()` without parameters will read from
   * the tied `Coord3D` source.
   * @param new_tie Pointer to the external accelerometer coordinate source.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord3D* new_tie)
  {
    tied_x = &(new_tie->x);
    tied_y = &(new_tie->y);
    tied_z = &(new_tie->z);

    return 1;
  }

private:
  const double* tied_x;
  const double* tied_y;
  const double* tied_z;
  double roll = 0;
  double tilt = 0;
  double magnitude = 0;
};

using three_dof_tilt_roll = Tilt_Roll;
using simple_tilt_roll = three_dof_tilt_roll;

} // namespace puara_gestures