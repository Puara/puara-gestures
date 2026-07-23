/**
* @file stillness.h
* @brief Detect when a device is held still and for how long, from an activity signal.
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
   * @class Stillness
   * @brief Detects held-still moments and how long they last — the inverse of
   * a motion detector.
   *
   * @details
   * Stopping is a gesture too: holding a pose, resting, settling after a
   * move. `Stillness` watches an activity signal (motion energy, a
   * @ref Shake output, |gyro|) and reports when it drops into quiet and for
   * how long, so you can freeze a parameter, snapshot a pose, arm a
   * calibration, or gate an effect on "the performer is holding". It is the
   * complement of @ref Segmenter, which marks the active phrases.
   *
   *   - `still`   : true while the activity is at/below `threshold`;
   *   - `settled` : true once it has been still for at least `settleTime` ms
   *                 (debounced "really stopped", not just a momentary dip);
   *   - `onset`   : true for one update when stillness begins;
   *   - `moved`   : true for one update when motion resumes;
   *   - `stillTime` : how long the current stillness has lasted, in ms.
   *
   * The still test is deterministic; `stillTime`/`settled` use the library
   * monotonic clock. It behaves identically in an Arduino `loop()`, a thread,
   * or an ossia score process. Header-only, doubles only, no STL/Boost.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/stillness.h>
   *
   * double activity = 0.0;
   * puara_gestures::Stillness stillness(&activity);
   * stillness.settleTime = 500; // ms of quiet before "settled"
   *
   * void loop() {
   *   activity = shake.current_value();
   *   stillness.update();
   *   if (stillness.settled) snapshotPose();
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `stillness.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Stillness
{
public:
  /**
   * @brief Default-construct a Stillness without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Stillness() noexcept
  : tied_data(nullptr)
  {
  }

  Stillness(const Stillness&) noexcept = default;
  Stillness(Stillness&&) noexcept = default;
  Stillness& operator=(const Stillness&) noexcept = default;
  Stillness& operator=(Stillness&&) noexcept = default;

  /**
   * @brief Construct a Stillness tied to external activity data.
   * @param tied Pointer to an external double source for the activity signal.
   */
  explicit Stillness(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Activity at or below this counts as still. */
  double threshold = 0.05;
  /** @brief Milliseconds of continuous stillness before `settled` turns true. */
  unsigned long settleTime = 500;

  // --- Outputs -------------------------------------------------------------

  /** @brief True while the activity is at/below `threshold`. */
  bool still = false;
  /** @brief True once stillness has lasted at least `settleTime` ms. */
  bool settled = false;
  /** @brief True for one update when stillness begins. */
  bool onset = false;
  /** @brief True for one update when motion resumes. */
  bool moved = false;
  /** @brief Duration of the current stillness in milliseconds (0 when moving). */
  double stillTime = 0.0;

  /**
   * @brief Update from a direct activity value.
   * @param value Current activity/energy level.
   * @return 1 while still, 0 otherwise.
   */
  int update(double value)
  {
    long now = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
    onset = false;
    moved = false;

    bool quiet = std::fabs(value) <= threshold;

    if(quiet)
    {
      if(!still)
      {
        still = true;
        onset = true;
        startTime = now;
      }
      stillTime = static_cast<double>(now - startTime);
      settled = static_cast<unsigned long>(stillTime) >= settleTime;
    }
    else
    {
      if(still)
      {
        moved = true;
      }
      still = false;
      settled = false;
      stillTime = 0.0;
    }
    return still ? 1 : 0;
  }

  /**
   * @brief Update from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return 1 while still, 0 otherwise.
   */
  int update(Coord1D reading) { return Stillness::update(reading.x); }

  /**
   * @brief Update from the tied external activity value.
   * @return 1 while still, 0 otherwise (also 0 when no tied input exists).
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      return Stillness::update(*tied_data);
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current stillness state.
   * @return 1.0 while still, 0.0 otherwise.
   */
  double current_value() const { return still ? 1.0 : 0.0; }

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
    still = false;
    settled = false;
    onset = false;
    moved = false;
    stillTime = 0.0;
    startTime = 0;
  }

private:
  const double* tied_data{};
  long startTime = 0;
};
}
