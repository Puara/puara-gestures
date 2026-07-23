/**
* @file envelope.h
* @brief Envelope follower: track the amplitude/energy of a signal with separate attack and release.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{
  /**
   * @class Envelope
   * @brief Envelope follower for a 1D signal, with independent attack and
   * release response.
   *
   * @details
   * An envelope follower tracks the amplitude/energy contour of a changing
   * signal: it rises quickly toward louder input and falls back slowly when
   * the input drops, exactly like the meter on an audio compressor. Feed it
   * anything that varies over time (sensor energy, a Shake output, rectified
   * accelerometer motion, breath pressure) and read a smooth loudness-style
   * value you can map to synthesis amplitude, brightness, or gesture
   * intensity.
   *
   * Unlike @ref puara_gestures::utils::LeakyIntegrator, which has a single
   * decay, the envelope uses two separate coefficients so the rise (attack)
   * and fall (release) can be shaped independently:
   *   - `attack`  : fraction the envelope moves toward a *rising* input each
   *                 update (0..1). 1.0 = jump instantly, small = slow swell.
   *   - `release` : fraction the envelope moves toward a *falling* input each
   *                 update (0..1). 1.0 = drop instantly, small = long decay.
   *   - `rectify` : when true (default) the input is taken as `|value|`, so a
   *                 bipolar signal (e.g. acceleration around 0) is followed by
   *                 its magnitude. Set false if the input is already positive.
   *
   * The coefficients are applied per `update()` call (like `LeakyIntegrator`'s
   * leak), so the follower is fully deterministic and does not depend on the
   * wall clock: it behaves identically whether driven from an Arduino
   * `loop()`, a thread, or an ossia score process, as long as it is called at
   * a roughly steady rate. Header-only, doubles only, no STL/Boost/allocation.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/envelope.h>
   *
   * double motion = 0.0;
   * puara_gestures::Envelope envelope(&motion); // Tied to external motion value
   *
   * void loop() {
   *   motion = readAccelerationMagnitude();
   *   envelope.update(); // Reads from tied motion automatically
   *   double loudness = envelope.current_value();
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `envelope.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Envelope
{
public:
  /**
   * @brief Default-construct an Envelope without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Envelope() noexcept
  : tied_data(nullptr)
  {
  }

  Envelope(const Envelope&) noexcept = default;
  Envelope(Envelope&&) noexcept = default;
  Envelope& operator=(const Envelope&) noexcept = default;
  Envelope& operator=(Envelope&&) noexcept = default;

  /**
   * @brief Construct an Envelope tied to external input data.
   *
   * When tied, calling `update()` reads the current value from `tied_data`.
   * @param tied Pointer to an external double source for the input signal.
   */
  explicit Envelope(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Rise coefficient in [0, 1]: how fast the envelope follows louder input. */
  double attack = 0.5;
  /** @brief Fall coefficient in [0, 1]: how fast the envelope follows quieter input. */
  double release = 0.1;
  /** @brief When true, follow the magnitude (|value|) of a bipolar input. */
  bool rectify = true;

  /**
   * @brief Update the envelope from a direct input value.
   *
   * Rectifies the input if configured, then moves the envelope toward it
   * using `attack` when rising or `release` when falling.
   * @param value Current input value.
   * @return The updated envelope value.
   */
  double update(double value)
  {
    double target = rectify ? std::fabs(value) : value;
    double coeff = (target > envelope) ? attack : release;
    envelope += coeff * (target - envelope);
    return envelope;
  }

  /**
   * @brief Update the envelope from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return The updated envelope value.
   */
  double update(Coord1D reading) { return Envelope::update(reading.x); }

  /**
   * @brief Update the envelope from the tied external input value.
   *
   * This overload reads the latest input from `tied_data` and processes it.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Envelope::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current envelope value.
   * @return The latest computed envelope.
   */
  double current_value() const { return envelope; }

  /**
   * @brief Connect the follower to an external double source.
   * @param new_tie Pointer to the external input value.
   * @return 1 when the tie succeeds.
   */
  int tie(double* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /**
   * @brief Connect the follower to an external `Coord1D` source.
   * @param new_tie Pointer to the external coordinate struct.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

  /** @brief Reset the envelope to zero. */
  void reset() { envelope = 0.0; }

private:
  const double* tied_data{};
  double envelope = 0.0;
};
}
