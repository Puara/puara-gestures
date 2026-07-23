/**
* @file heading.h
* @brief Tilt-compensated compass heading (yaw) from accelerometer and magnetometer data.
* @see https://github.com/Puara/puara-gestures
* @see Freescale AN4248 - "Implementing a Tilt-Compensated eCompass"
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>
#include <numbers>

namespace puara_gestures
{
  /**
   * @class Heading
   * @brief Compass heading (yaw / azimuth) with tilt compensation.
   *
   * @details
   * `Heading` gives the missing third orientation axis. @ref Tilt and
   * @ref Roll cover two of the three degrees of freedom; `Heading` computes
   * the yaw/azimuth — which way the device points around the vertical — from
   * the magnetometer, tilt-compensated with the accelerometer so the reading
   * stays stable even when the device is not held flat. Together they give a
   * full 3D pointing direction for free-space instruments, spatial mapping,
   * or "point at a speaker/light" control.
   *
   * The math follows the standard tilt-compensated eCompass (Freescale
   * AN4248): roll and pitch are estimated from acceleration, the magnetic
   * vector is rotated back into the horizontal plane, and the heading is the
   * angle of that horizontal vector. `declination` is added so you can turn
   * magnetic north into true north for your location.
   *
   * Outputs `heading` in degrees [0, 360) and `heading_radians` in [0, 2*pi),
   * plus the intermediate `roll`/`pitch` (radians) it derived. It needs the
   * accelerometer and magnetometer only (gyroscope unused). Header-only,
   * doubles only, no STL/Boost/allocation, so it behaves identically in an
   * Arduino `loop()`, a thread, or an ossia score process.
   *
   * @note The magnetometer must be calibrated (hard/soft-iron) for the
   * heading to be accurate; see the magnetometer calibration utilities.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/heading.h>
   *
   * puara_gestures::Imu9Axis imu;
   * puara_gestures::Heading heading(&imu);
   *
   * void loop() {
   *   imu.accl = readAccel();
   *   imu.magn = readMag();
   *   heading.update(); // reads accl + magn from the tied imu
   *   double bearing = heading.heading; // degrees, 0 = magnetic north
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `heading.update(accl, magn)`
   * directly.
   *
   * @ingroup puara_gestures_descriptors
   */
class Heading
{
public:
  /**
   * @brief Default-construct a Heading without tied input data.
   *
   * Use `update(accl, magn)` to provide input directly.
   */
  Heading() noexcept
  : tied_data(nullptr)
  {
  }

  Heading(const Heading&) noexcept = default;
  Heading(Heading&&) noexcept = default;
  Heading& operator=(const Heading&) noexcept = default;
  Heading& operator=(Heading&&) noexcept = default;

  /**
   * @brief Construct a Heading tied to an external `Imu9Axis` source.
   * @param tied Pointer to an external IMU sample (accl + magn are used).
   */
  explicit Heading(Imu9Axis* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Magnetic declination in degrees, added to map magnetic to true north. */
  double declination = 0.0;

  // --- Outputs -------------------------------------------------------------

  /** @brief Compass heading in degrees, [0, 360), 0 = (magnetic) north. */
  double heading = 0.0;
  /** @brief Compass heading in radians, [0, 2*pi). */
  double heading_radians = 0.0;
  /** @brief Roll angle derived from acceleration, in radians. */
  double roll = 0.0;
  /** @brief Pitch angle derived from acceleration, in radians. */
  double pitch = 0.0;

  /**
   * @brief Update the heading from accelerometer and magnetometer vectors.
   * @param accl Acceleration vector (units cancel; direction is what matters).
   * @param magn Magnetic field vector in the same body frame as `accl`.
   * @return The updated heading in degrees.
   */
  double update(Coord3D accl, Coord3D magn)
  {
    // Roll (phi) and pitch (theta) from the gravity direction.
    roll = std::atan2(accl.y, accl.z);
    pitch = std::atan2(-accl.x, accl.y * std::sin(roll) + accl.z * std::cos(roll));

    // Rotate the magnetic vector back into the horizontal plane.
    double bfx = magn.x * std::cos(pitch) + magn.y * std::sin(pitch) * std::sin(roll)
        + magn.z * std::sin(pitch) * std::cos(roll);
    double bfy = magn.z * std::sin(roll) - magn.y * std::cos(roll);

    double yaw = std::atan2(bfy, bfx);  // radians, (-pi, pi]

    double degrees = yaw * (180.0 / pi) + declination;
    degrees = std::fmod(degrees, 360.0);
    if(degrees < 0.0)
    {
      degrees += 360.0;
    }
    heading = degrees;
    heading_radians = degrees * (pi / 180.0);
    return heading;
  }

  /**
   * @brief Update the heading from an `Imu9Axis` sample (uses accl + magn).
   * @param imu Nine-axis IMU sample.
   * @return The updated heading in degrees.
   */
  double update(Imu9Axis imu) { return Heading::update(imu.accl, imu.magn); }

  /**
   * @brief Update the heading from the tied external IMU sample.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Heading::update(tied_data->accl, tied_data->magn);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current heading.
   * @return The latest heading in degrees.
   */
  double current_value() const { return heading; }

  /**
   * @brief Connect to an external `Imu9Axis` source.
   * @param new_tie Pointer to the external IMU sample.
   * @return 1 when the tie succeeds.
   */
  int tie(Imu9Axis* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /** @brief Reset the computed outputs. */
  void reset()
  {
    heading = 0.0;
    heading_radians = 0.0;
    roll = 0.0;
    pitch = 0.0;
  }

private:
  static constexpr double pi = std::numbers::pi;

  const Imu9Axis* tied_data{};
};
}
