/**
* @file freefall.h
* @brief Detect free-fall (near-weightless) moments and their duration from acceleration.
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
   * @class Freefall
   * @brief Detects free-fall: the near-weightless moment when acceleration
   * magnitude drops toward zero.
   *
   * @details
   * A resting accelerometer reads ~1 g (gravity). When the device is dropped,
   * thrown, or tossed, it briefly becomes weightless and the acceleration
   * magnitude falls toward 0. `Freefall` watches that magnitude and flags the
   * weightless window, timing how long it lasts — a playful, expressive
   * trigger for throw-to-play, catch, or drop-activated gestures.
   *
   *   - `freefall` : true while the magnitude is at/below `threshold`;
   *   - `onset`    : true for one update when free-fall begins (the drop);
   *   - `landed`   : true for one update when it ends (caught/landed);
   *   - `fallTime` : how long the current free-fall has lasted, in ms;
   *   - `magnitude`: the acceleration magnitude last evaluated.
   *
   * Feed it a `Coord3D` acceleration (the magnitude is computed for you) or a
   * pre-computed magnitude. `threshold` is in the same units as the input:
   * with acceleration in g, ~0.3 works well; scale up for m/s^2. The
   * free-fall test is deterministic; `fallTime` uses the library monotonic
   * clock. It behaves identically in an Arduino `loop()`, a thread, or an
   * ossia score process. Header-only, doubles only, no STL/Boost.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/freefall.h>
   *
   * puara_gestures::Coord3D accel{};
   * puara_gestures::Freefall freefall(&accel);
   * freefall.threshold = 0.3; // g
   *
   * void loop() {
   *   accel = readAcceleration(); // in g
   *   freefall.update();
   *   if (freefall.onset)  startSound();
   *   if (freefall.landed) stopSound();
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `freefall.update(accel)` or
   * `freefall.update(magnitude)` directly.
   *
   * @ingroup puara_gestures_descriptors
   */
class Freefall
{
public:
  /**
   * @brief Default-construct a Freefall without tied input data.
   *
   * Use `update(accel)` or `update(magnitude)` to provide input directly.
   */
  Freefall() noexcept
  : tied_data(nullptr)
  {
  }

  Freefall(const Freefall&) noexcept = default;
  Freefall(Freefall&&) noexcept = default;
  Freefall& operator=(const Freefall&) noexcept = default;
  Freefall& operator=(Freefall&&) noexcept = default;

  /**
   * @brief Construct a Freefall tied to an external `Coord3D` acceleration.
   * @param tied Pointer to the external acceleration vector.
   */
  explicit Freefall(Coord3D* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Acceleration magnitude at or below this counts as free-fall (same units as input). */
  double threshold = 0.3;

  // --- Outputs -------------------------------------------------------------

  /** @brief True while in free-fall (magnitude at/below threshold). */
  bool freefall = false;
  /** @brief True for one update when free-fall begins. */
  bool onset = false;
  /** @brief True for one update when free-fall ends (landed/caught). */
  bool landed = false;
  /** @brief Duration of the current free-fall in milliseconds (0 when not falling). */
  double fallTime = 0.0;
  /** @brief Acceleration magnitude last evaluated. */
  double magnitude = 0.0;

  /**
   * @brief Update from a pre-computed acceleration magnitude.
   * @param mag Current acceleration magnitude.
   * @return 1 while in free-fall, 0 otherwise.
   */
  double update(double mag)
  {
    long now = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
    magnitude = mag;
    onset = false;
    landed = false;

    if(mag <= threshold)
    {
      if(!freefall)
      {
        freefall = true;
        onset = true;
        startTime = now;
      }
      fallTime = static_cast<double>(now - startTime);
    }
    else
    {
      if(freefall)
      {
        landed = true;
      }
      freefall = false;
      fallTime = 0.0;
    }
    return freefall ? 1.0 : 0.0;
  }

  /**
   * @brief Update from a `Coord3D` acceleration (magnitude computed here).
   * @param accel Acceleration vector.
   * @return 1 while in free-fall, 0 otherwise.
   */
  double update(Coord3D accel)
  {
    double mag
        = std::sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
    return Freefall::update(mag);
  }

  /**
   * @brief Update from the tied external acceleration vector.
   * @return 1 while in free-fall, 0 otherwise (also 0 when no tied input exists).
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Freefall::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current free-fall state.
   * @return 1.0 while in free-fall, 0.0 otherwise.
   */
  double current_value() const { return freefall ? 1.0 : 0.0; }

  /**
   * @brief Connect to an external `Coord3D` acceleration source.
   * @param new_tie Pointer to the external acceleration vector.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord3D* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /** @brief Clear all state, keeping the configuration. */
  void reset()
  {
    freefall = false;
    onset = false;
    landed = false;
    fallTime = 0.0;
    magnitude = 0.0;
    startTime = 0;
  }

private:
  const Coord3D* tied_data{};
  long startTime = 0;
};
}
