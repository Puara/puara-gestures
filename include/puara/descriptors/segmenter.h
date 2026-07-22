/**
* @file segmenter.h
* @brief Segment a continuous activity signal into movement phrases (onset/offset + duration).
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
   * @class Segmenter
   * @brief Splits a continuous activity/energy signal into discrete movement
   * phrases, reporting when each one starts, ends, and how long it lasts.
   *
   * @details
   * Expressive playing comes in phrases — a gesture, a stroke, a held move —
   * separated by quiet. `Segmenter` watches an activity signal (motion
   * energy, a @ref Shake output, |gyro|, breath pressure) and marks the
   * boundaries of each active phrase, so you can trigger envelopes, arm a
   * recorder, or count gestures without hand-writing state machines.
   *
   * It uses hysteresis to stay robust to noise (a phrase starts when the
   * signal rises to `onThreshold` and ends when it falls to `offThreshold`),
   * plus optional debouncing to reject blips and gaps:
   *   - `active`   : true during a phrase;
   *   - `onset`    : true for one update when a phrase begins;
   *   - `offset`   : true for one update when a phrase ends;
   *   - `duration` : length of the current (or just-finished) phrase in ms;
   *   - `samples`  : the same length counted in `update()` calls;
   *   - `count`    : how many phrases have completed.
   *
   * The hysteresis gate is deterministic; `duration` uses the library
   * monotonic clock. It behaves identically in an Arduino `loop()`, a thread,
   * or an ossia score process. Header-only, doubles only, no STL/Boost.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/segmenter.h>
   *
   * double energy = 0.0;
   * puara_gestures::Segmenter phrase(&energy);
   *
   * void loop() {
   *   energy = shake.current_value();
   *   phrase.update();
   *   if (phrase.onset)  startEnvelope();
   *   if (phrase.offset) releaseEnvelope();
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `phrase.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Segmenter
{
public:
  /**
   * @brief Default-construct a Segmenter without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Segmenter() noexcept
  : tied_data(nullptr)
  {
  }

  Segmenter(const Segmenter&) noexcept = default;
  Segmenter(Segmenter&&) noexcept = default;
  Segmenter& operator=(const Segmenter&) noexcept = default;
  Segmenter& operator=(Segmenter&&) noexcept = default;

  /**
   * @brief Construct a Segmenter tied to external activity data.
   * @param tied Pointer to an external double source for the activity signal.
   */
  explicit Segmenter(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Activity level that starts a phrase. */
  double onThreshold = 0.1;
  /** @brief Activity level that ends a phrase; keep below onThreshold. */
  double offThreshold = 0.05;
  /** @brief Consecutive above-threshold updates required to confirm an onset (debounce). */
  unsigned int minActiveSamples = 1;
  /** @brief Consecutive below-threshold updates required to confirm an offset (debounce). */
  unsigned int minSilentSamples = 1;

  // --- Outputs -------------------------------------------------------------

  /** @brief True while a movement phrase is in progress. */
  bool active = false;
  /** @brief True for one update when a phrase begins. */
  bool onset = false;
  /** @brief True for one update when a phrase ends. */
  bool offset = false;
  /** @brief Length of the current/just-finished phrase in milliseconds. */
  double duration = 0.0;
  /** @brief Length of the current/just-finished phrase in update() calls. */
  unsigned int samples = 0;
  /** @brief Number of completed phrases since construction/reset. */
  unsigned int count = 0;

  /**
   * @brief Update the segmenter from a direct activity value.
   * @param value Current activity/energy level.
   * @return 1 while active, 0 otherwise.
   */
  int update(double value)
  {
    long now = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
    onset = false;
    offset = false;

    if(!active)
    {
      // Waiting to confirm a phrase start.
      if(value >= onThreshold)
      {
        aboveCount++;
        if(aboveCount >= minActiveSamples)
        {
          active = true;
          onset = true;
          startTime = now;
          samples = 0;
          belowCount = 0;
        }
      }
      else
      {
        aboveCount = 0;
      }
    }
    else
    {
      samples++;
      duration = static_cast<double>(now - startTime);
      // Waiting to confirm a phrase end.
      if(value <= offThreshold)
      {
        belowCount++;
        if(belowCount >= minSilentSamples)
        {
          active = false;
          offset = true;
          count++;
          aboveCount = 0;
        }
      }
      else
      {
        belowCount = 0;
      }
    }
    return active ? 1 : 0;
  }

  /**
   * @brief Update the segmenter from the tied external activity value.
   * @return 1 while active, 0 otherwise (also 0 when no tied input exists).
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      return Segmenter::update(*tied_data);
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Update the segmenter from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return 1 while active, 0 otherwise.
   */
  int update(Coord1D reading) { return Segmenter::update(reading.x); }

  /**
   * @brief Get the current phrase state.
   * @return 1.0 while a phrase is active, 0.0 otherwise.
   */
  double current_value() const { return active ? 1.0 : 0.0; }

  /**
   * @brief Connect to an external double source.
   * @param new_tie Pointer to the external activity value.
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

  /** @brief Clear all state, keeping the configuration. */
  void reset()
  {
    active = false;
    onset = false;
    offset = false;
    duration = 0.0;
    samples = 0;
    count = 0;
    aboveCount = 0;
    belowCount = 0;
    startTime = 0;
  }

private:
  const double* tied_data{};
  unsigned int aboveCount = 0;
  unsigned int belowCount = 0;
  long startTime = 0;
};
}
