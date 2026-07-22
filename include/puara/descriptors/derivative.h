/**
* @file derivative.h
* @brief Velocity, acceleration and jerk from a continuous position signal via finite differences.
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
   * @class Derivative
   * @brief Rate-of-change features of a continuous signal: velocity,
   * acceleration and jerk.
   *
   * @details
   * Many sensors report a *position*-like quantity (a slider, a distance, an
   * angle, a smoothed touch coordinate) while the expressive information is
   * in how fast it is *changing*. `Derivative` differentiates the incoming
   * value over time to give velocity (1st derivative), acceleration (2nd),
   * and jerk (3rd) — the classic "how fast / speeding up / snap" trio that
   * makes position sensors feel alive when mapped to synthesis.
   *
   * Derivatives amplify noise, so smooth the input first (e.g. with
   * @ref puara_gestures::utils::OneEuro) or the `velocity` output afterward
   * for musical results. Timing comes from the library monotonic clock
   * (`update(value)`) or an explicit step (`update(value, dt)`), so it
   * behaves identically in an Arduino `loop()`, a thread, or an ossia score
   * process. Header-only, doubles only, no STL/Boost/allocation.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/derivative.h>
   *
   * double position = 0.0;
   * puara_gestures::Derivative derivative(&position);
   *
   * void loop() {
   *   position = readSlider();
   *   derivative.update();
   *   double speed = derivative.velocity;
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `derivative.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Derivative
{
public:
  /**
   * @brief Default-construct a Derivative without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Derivative() noexcept
  : tied_data(nullptr)
  {
  }

  Derivative(const Derivative&) noexcept = default;
  Derivative(Derivative&&) noexcept = default;
  Derivative& operator=(const Derivative&) noexcept = default;
  Derivative& operator=(Derivative&&) noexcept = default;

  /**
   * @brief Construct a Derivative tied to external position data.
   * @param tied Pointer to an external double source for the input signal.
   */
  explicit Derivative(double* tied)
  : tied_data(tied)
  {
  }

  // --- Outputs -------------------------------------------------------------

  /** @brief Most recent input value. */
  double position = 0.0;
  /** @brief First derivative: rate of change per second. */
  double velocity = 0.0;
  /** @brief Second derivative: change of velocity per second. */
  double acceleration = 0.0;
  /** @brief Third derivative: change of acceleration per second. */
  double jerk = 0.0;

  /**
   * @brief Update from a value, timing the step with the monotonic clock.
   * @param value Current input value.
   * @return The updated velocity.
   */
  double update(double value)
  {
    unsigned long long now = puara_gestures::utils::getCurrentTimeMicroseconds();
    double dt = 0.0;
    if(initialized && now > lastTime)
    {
      dt = static_cast<double>(now - lastTime) / 1e6;
    }
    lastTime = now;
    return differentiate(value, dt);
  }

  /**
   * @brief Update from a value with an explicit time step.
   * @param value Current input value.
   * @param dt Time elapsed since the previous sample, in seconds.
   * @return The updated velocity.
   */
  double update(double value, double dt)
  {
    if(dt < 0.0)
    {
      dt = 0.0;
    }
    return differentiate(value, dt);
  }

  /**
   * @brief Update from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return The updated velocity.
   */
  double update(Coord1D reading) { return Derivative::update(reading.x); }

  /**
   * @brief Update from the tied external value.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Derivative::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current velocity.
   * @return The latest first derivative.
   */
  double current_value() const { return velocity; }

  /**
   * @brief Connect to an external double source.
   * @param new_tie Pointer to the external value.
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

  /** @brief Clear all state, keeping any tie. */
  void reset()
  {
    position = 0.0;
    velocity = 0.0;
    acceleration = 0.0;
    jerk = 0.0;
    initialized = false;
    lastTime = 0;
    lastVelocity = 0.0;
    lastAcceleration = 0.0;
  }

private:
  double differentiate(double value, double dt)
  {
    position = value;
    if(!initialized || dt <= 0.0)
    {
      // First sample (or no elapsed time): anchor without producing spikes.
      initialized = true;
      lastValue = value;
      return velocity;
    }

    double newVelocity = (value - lastValue) / dt;
    double newAcceleration = (newVelocity - lastVelocity) / dt;
    jerk = (newAcceleration - lastAcceleration) / dt;
    acceleration = newAcceleration;
    velocity = newVelocity;

    lastValue = value;
    lastVelocity = newVelocity;
    lastAcceleration = newAcceleration;
    return velocity;
  }

  const double* tied_data{};
  bool initialized = false;
  unsigned long long lastTime = 0;
  double lastValue = 0.0;
  double lastVelocity = 0.0;
  double lastAcceleration = 0.0;
};
}
