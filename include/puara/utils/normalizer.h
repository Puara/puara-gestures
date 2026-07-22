/**
* @file normalizer.h
* @brief Auto-ranging normalizer: map a signal of unknown range to [0, 1] from its own running extremes.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <cmath>

namespace puara_gestures::utils
{
/**
 * @class Normalizer
 * @brief Rescales an incoming signal to a fixed output range using the
 * min and max it has actually seen — no manual calibration.
 *
 * @details
 * Every sensor has a different, often unknown, range. `Normalizer` learns it
 * on the fly: it tracks the smallest and largest values observed and maps the
 * current value to `[outMin, outMax]` (0..1 by default) accordingly. Drop it
 * in front of a mapping and a raw sensor immediately spans the full control
 * range without hard-coded limits, which is ideal for quick instrument
 * prototyping and self-calibrating controllers.
 *
 * An optional `decay` in [0, 1) slowly relaxes the learned range back toward
 * the current value each update, so stale extremes (a one-off spike, or a
 * range that has since narrowed) fade and the mapping keeps adapting. With
 * `decay = 0` the range only ever expands (classic peak-hold auto-range).
 * `calibrating = false` freezes the learned range so the mapping stops
 * adapting once you are happy with it.
 *
 * No timing or allocation, arithmetic only, so it is fully deterministic and
 * behaves identically in an Arduino `loop()`, a thread, or an ossia score
 * process. Header-only, no STL/Boost. For a windowed min/max instead of an
 * all-time one, see @ref RollingMinMax.
 *
 * Example:
 * @code
 *   puara_gestures::utils::Normalizer norm;   // outputs 0..1
 *   double control = norm.update(analogRead(A0));
 * @endcode
 */
class Normalizer
{
public:
  /** @brief Lower bound of the output range. */
  double outMin = 0.0;
  /** @brief Upper bound of the output range. */
  double outMax = 1.0;
  /** @brief Per-update relaxation of the learned range in [0, 1); 0 = expand-only. */
  double decay = 0.0;
  /** @brief When false, the learned min/max are frozen (stops adapting). */
  bool calibrating = true;

  /** @brief Most recent normalized output. */
  double current_value = 0.0;
  /** @brief Smallest input value seen while calibrating. */
  double min = 0.0;
  /** @brief Largest input value seen while calibrating. */
  double max = 0.0;

  Normalizer() noexcept = default;
  Normalizer(const Normalizer&) noexcept = default;
  Normalizer(Normalizer&&) noexcept = default;
  Normalizer& operator=(const Normalizer&) noexcept = default;
  Normalizer& operator=(Normalizer&&) noexcept = default;

  /**
   * @brief Construct with an explicit output range.
   * @param outMinValue Lower bound of the output.
   * @param outMaxValue Upper bound of the output.
   */
  Normalizer(double outMinValue, double outMaxValue) noexcept
  : outMin(outMinValue)
  , outMax(outMaxValue)
  {
  }

  /**
   * @brief Feed a new sample and get its normalized value.
   * @param value Current raw input.
   * @return The value mapped into [outMin, outMax].
   */
  double update(double value)
  {
    if(!initialized)
    {
      initialized = true;
      min = value;
      max = value;
      current_value = outMin;
      return current_value;
    }

    if(calibrating)
    {
      if(value < min)
      {
        min = value;
      }
      if(value > max)
      {
        max = value;
      }
      // Relax stale extremes toward the current value so the range keeps adapting.
      if(decay > 0.0)
      {
        min += (value - min) * decay;
        max += (value - max) * decay;
      }
    }

    double span = max - min;
    double normalized = (span > epsilon) ? (value - min) / span : 0.0;
    if(normalized < 0.0)
    {
      normalized = 0.0;
    }
    if(normalized > 1.0)
    {
      normalized = 1.0;
    }
    current_value = outMin + normalized * (outMax - outMin);
    return current_value;
  }

  /** @brief Clear the learned range; the next update() re-seeds it. */
  void reset()
  {
    initialized = false;
    min = 0.0;
    max = 0.0;
    current_value = outMin;
  }

private:
  static constexpr double epsilon = 1e-12;

  bool initialized = false;
};
}
