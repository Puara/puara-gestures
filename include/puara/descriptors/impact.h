/**
* @file impact.h
* @brief Detect sharp impacts/strikes and their intensity from a magnitude signal.
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
   * @class Impact
   * @brief Percussive hit detector: fires once per strike and reports its
   * intensity from a positive magnitude signal.
   *
   * @details
   * Feed `Impact` a positive magnitude that spikes on a hit — accelerometer
   * magnitude, a piezo reading, the energy of a contact sensor — and it
   * turns each spike into a single trigger plus a strength value, the way a
   * drum pad turns a physical hit into a note-on with velocity. It is the
   * percussive counterpart to @ref Jab (which reports a continuous motion
   * range rather than a debounced, one-shot trigger).
   *
   * Detection is edge + hysteresis based, so one hit produces exactly one
   * `strike`:
   *   - a strike fires the moment the input rises to `threshold` (low
   *     latency, on the onset — not on release);
   *   - `intensity` holds the peak magnitude reached during that hit;
   *   - the detector re-arms only after the input falls back below
   *     `releaseThreshold` (hysteresis prevents a single messy hit from
   *     re-triggering), and after `minInterval` ms have passed if set
   *     (a refractory window that debounces bounce/ringing).
   *
   * Set `minInterval` to 0 to debounce purely by hysteresis (fully
   * deterministic, no timing). Otherwise timing uses the library monotonic
   * clock, so behaviour is identical in an Arduino `loop()`, a thread, or an
   * ossia score process. Header-only, doubles only, no STL/Boost/allocation.
   *
   * @note Feed a *positive* signal. For a bipolar source (e.g. acceleration
   * around 0) pass its magnitude, or rectify it first with an @ref Envelope.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/impact.h>
   *
   * double accel_mag = 0.0;
   * puara_gestures::Impact impact(&accel_mag); // Tied to external magnitude
   *
   * void loop() {
   *   accel_mag = readAccelerationMagnitude();
   *   impact.update(); // Reads from tied accel_mag automatically
   *   if (impact.strike) {
   *     triggerNote(impact.intensity); // one note-on per hit, with velocity
   *   }
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `impact.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Impact
{
public:
  /**
   * @brief Default-construct an Impact without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Impact() noexcept
  : tied_data(nullptr)
  {
  }

  Impact(const Impact&) noexcept = default;
  Impact(Impact&&) noexcept = default;
  Impact& operator=(const Impact&) noexcept = default;
  Impact& operator=(Impact&&) noexcept = default;

  /**
   * @brief Construct an Impact tied to external input data.
   *
   * When tied, calling `update()` reads the current value from `tied_data`.
   * @param tied Pointer to an external double source for the magnitude signal.
   */
  explicit Impact(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Magnitude the input must reach to fire a strike. */
  double threshold = 1.0;
  /** @brief Magnitude the input must fall below before another strike can fire. */
  double releaseThreshold = 0.5;
  /** @brief Minimum time between strikes in ms (refractory); 0 = hysteresis only. */
  unsigned long minInterval = 0;

  // --- Outputs -------------------------------------------------------------

  /** @brief True for exactly one update() call when a strike is detected. */
  bool strike = false;
  /** @brief Peak magnitude reached during the current/last strike (its "velocity"). */
  double intensity = 0.0;
  /** @brief Total number of strikes detected since construction/reset. */
  unsigned int count = 0;

  /**
   * @brief Update the detector from a direct magnitude value.
   * @param value Current positive magnitude input.
   * @return The current `intensity` (peak of the ongoing/last strike).
   */
  double update(double value)
  {
    long now = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
    strike = false;

    if(armed)
    {
      if(value >= threshold)
      {
        strike = true;
        count++;
        intensity = value;
        lastStrikeTime = now;
        armed = false;
        active = true;
      }
    }
    else
    {
      // Still inside the hit: keep the peak as the reported intensity.
      if(active && value > intensity)
      {
        intensity = value;
      }
      bool intervalElapsed = (minInterval == 0)
          || (static_cast<unsigned long>(now - lastStrikeTime) >= minInterval);
      if(value < releaseThreshold && intervalElapsed)
      {
        armed = true;
        active = false;
      }
    }
    return intensity;
  }

  /**
   * @brief Update the detector from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return The current `intensity`.
   */
  double update(Coord1D reading) { return Impact::update(reading.x); }

  /**
   * @brief Update the detector from the tied external input value.
   *
   * This overload reads the latest input from `tied_data` and processes it.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Impact::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current strike intensity.
   * @return The peak magnitude of the current/last strike.
   */
  double current_value() const { return intensity; }

  /**
   * @brief Connect the detector to an external double source.
   * @param new_tie Pointer to the external magnitude value.
   * @return 1 when the tie succeeds.
   */
  int tie(double* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /**
   * @brief Connect the detector to an external `Coord1D` source.
   * @param new_tie Pointer to the external coordinate struct.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

  /** @brief Clear all state, keeping the current configuration. */
  void reset()
  {
    strike = false;
    intensity = 0.0;
    count = 0;
    armed = true;
    active = false;
    lastStrikeTime = 0;
  }

private:
  const double* tied_data{};
  bool armed = true;      ///< Ready to fire the next strike.
  bool active = false;    ///< Currently inside a hit (tracking its peak).
  long lastStrikeTime = 0;
};
}
