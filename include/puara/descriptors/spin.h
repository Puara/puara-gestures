/**
* @file spin.h
* @brief Angular-velocity gestures: rotation speed, accumulated angle, and turn counting from gyroscope data.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{
  /**
   * @class Spin
   * @brief Rotation gesture from a single angular-velocity axis.
   *
   * @details
   * `Spin` turns a gyroscope rate (how fast the device is rotating about one
   * axis) into higher-level rotation features: the signed speed, the total
   * angle swept since the last reset, how many full turns that is, and the
   * current direction. Where @ref Tilt and @ref Roll report a static pose,
   * `Spin` reports *motion about an axis* — useful for twist/crank/knob-style
   * control, spin-to-scrub, or counting revolutions of a hand-held object.
   *
   * The angle is obtained by integrating the rate over time, so the value is
   * expressed in whatever unit the input uses. `revolution` says how many of
   * those units make one full turn (default 360 for degrees/second input;
   * set it to 2*pi for radians/second), which drives `revolutions` and the
   * integer `turns` count. `deadzone` suppresses small rates as noise so a
   * resting device does not slowly drift its angle.
   *
   * Timing comes from the library monotonic clock (`update(rate)`) or an
   * explicit step (`update(rate, dt)`), so it behaves identically in an
   * Arduino `loop()`, a thread, or an ossia score process. Header-only,
   * doubles only, no STL/Boost/allocation.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/spin.h>
   *
   * double gyro_z = 0.0; // degrees/second about the vertical axis
   * puara_gestures::Spin spin(&gyro_z);
   *
   * void loop() {
   *   gyro_z = readGyroZ();
   *   spin.update(); // integrates using the monotonic clock
   *   double speed = spin.velocity;    // signed rate
   *   long   turns = spin.turns;       // completed full turns
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `spin.update(rate)` directly.
   *
   * @ingroup puara_gestures_descriptors
   */
class Spin
{
public:
  /**
   * @brief Default-construct a Spin without tied input data.
   *
   * Use `update(rate)` to provide input directly.
   */
  Spin() noexcept
  : tied_data(nullptr)
  {
  }

  Spin(const Spin&) noexcept = default;
  Spin(Spin&&) noexcept = default;
  Spin& operator=(const Spin&) noexcept = default;
  Spin& operator=(Spin&&) noexcept = default;

  /**
   * @brief Construct a Spin tied to external angular-velocity data.
   * @param tied Pointer to an external double source for the rotation rate.
   */
  explicit Spin(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Input units that make one full turn (360 for deg/s, 2*pi for rad/s). */
  double revolution = 360.0;
  /** @brief Rates with magnitude at or below this are treated as zero (noise). */
  double deadzone = 0.0;

  // --- Outputs -------------------------------------------------------------

  /** @brief Most recent signed angular velocity (same unit as input). */
  double velocity = 0.0;
  /** @brief Accumulated signed angle swept since construction/reset. */
  double angle = 0.0;
  /** @brief Accumulated angle expressed in full turns (fractional, signed). */
  double revolutions = 0.0;
  /** @brief Completed whole turns (signed, truncated toward zero). */
  long turns = 0;
  /** @brief Direction of rotation: 1 = positive, -1 = negative, 0 = still. */
  int direction = 0;

  /**
   * @brief Update from a rate value, timing the step with the monotonic clock.
   * @param rate Current angular velocity.
   * @return The updated signed velocity.
   */
  double update(double rate)
  {
    unsigned long long now = puara_gestures::utils::getCurrentTimeMicroseconds();
    double dt = 0.0;
    if(initialized && now > lastTime)
    {
      dt = static_cast<double>(now - lastTime) / 1e6;
    }
    lastTime = now;
    initialized = true;
    return integrate(rate, dt);
  }

  /**
   * @brief Update from a rate value with an explicit time step.
   * @param rate Current angular velocity.
   * @param dt Time elapsed since the previous sample, in seconds.
   * @return The updated signed velocity.
   */
  double update(double rate, double dt)
  {
    if(dt < 0.0)
    {
      dt = 0.0;
    }
    return integrate(rate, dt);
  }

  /**
   * @brief Update from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used as the rate.
   * @return The updated signed velocity.
   */
  double update(Coord1D reading) { return Spin::update(reading.x); }

  /**
   * @brief Update from the tied external rate value.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Spin::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current angular velocity.
   * @return The latest signed rate.
   */
  double current_value() const { return velocity; }

  /**
   * @brief Connect to an external double source.
   * @param new_tie Pointer to the external rate value.
   * @return 1 when the tie succeeds.
   */
  int tie(double* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /**
   * @brief Connect to an external `Coord1D` source.
   * @param new_tie Pointer to the external coordinate struct.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

  /** @brief Clear accumulated angle and state, keeping the configuration. */
  void reset()
  {
    velocity = 0.0;
    angle = 0.0;
    revolutions = 0.0;
    turns = 0;
    direction = 0;
    initialized = false;
    lastTime = 0;
  }

private:
  double integrate(double rate, double dt)
  {
    if(std::fabs(rate) <= deadzone)
    {
      rate = 0.0;
    }
    velocity = rate;
    angle += rate * dt;
    revolutions = (revolution != 0.0) ? angle / revolution : 0.0;
    turns = static_cast<long>(revolutions);  // truncates toward zero
    direction = (rate > 0.0) ? 1 : (rate < 0.0) ? -1 : 0;
    return velocity;
  }

  const double* tied_data{};
  bool initialized = false;
  unsigned long long lastTime = 0;
};

/**
 * @class Spin3D
 * @brief Rotation gesture for three angular-velocity axes at once.
 *
 * @details
 * Spin3D runs a @ref Spin on each of the X/Y/Z gyroscope axes and adds the
 * overall rotation speed (the magnitude of the angular-velocity vector),
 * handy when you care how fast something is turning regardless of axis.
 *
 * @ingroup puara_gestures_descriptors
 */
class Spin3D
{
public:
  Spin x{}, y{}, z{};

  Spin3D() = default;

  /**
   * @brief Construct tied to an external `Coord3D` gyro source.
   * @param tied Pointer to the external coordinate struct.
   */
  explicit Spin3D(Coord3D* tied)
  : tied_data(tied)
  {
    x.tie(&(tied->x));
    y.tie(&(tied->y));
    z.tie(&(tied->z));
  }

  /** @brief Set the same full-turn scale on all three axes. */
  void set_revolution(double value)
  {
    x.revolution = value;
    y.revolution = value;
    z.revolution = value;
  }

  /** @brief Set the same deadzone on all three axes. */
  void set_deadzone(double value)
  {
    x.deadzone = value;
    y.deadzone = value;
    z.deadzone = value;
  }

  /**
   * @brief Update all three axes from explicit rates and time step.
   * @return The overall rotation speed (vector magnitude).
   */
  double update(double rx, double ry, double rz, double dt)
  {
    x.update(rx, dt);
    y.update(ry, dt);
    z.update(rz, dt);
    return magnitude();
  }

  /**
   * @brief Update all three axes from a `Coord3D`, timing with the clock.
   * @return The overall rotation speed (vector magnitude).
   */
  double update(Coord3D reading)
  {
    x.update(reading.x);
    y.update(reading.y);
    z.update(reading.z);
    return magnitude();
  }

  /**
   * @brief Update from the tied external `Coord3D`, timing with the clock.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      x.update();
      y.update();
      z.update();
      return 1;
    }
    return 0;
  }

  /** @brief Per-axis signed angular velocities. */
  Coord3D current_value() const { return Coord3D{x.velocity, y.velocity, z.velocity}; }

  /** @brief Overall rotation speed: magnitude of the angular-velocity vector. */
  double magnitude() const
  {
    return std::sqrt(
        x.velocity * x.velocity + y.velocity * y.velocity + z.velocity * z.velocity);
  }

  /** @brief Reset all three axes. */
  void reset()
  {
    x.reset();
    y.reset();
    z.reset();
  }

private:
  Coord3D* tied_data{};
};
}
