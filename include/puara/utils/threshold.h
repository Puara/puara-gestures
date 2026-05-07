//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once


#include <puara/structs.h>
#include <cmath>
#include <type_traits>

namespace puara_gestures::utils
{

/**
 * @brief Clamp a numeric value to a configurable range.
 *
 * ThresholdT is a lightweight value clamp helper. It stores a minimum and
 * maximum boundary, applies clamping through `update()`, and keeps the last
 * raw input in `current`.
 *
 * The legacy type alias `Threshold` preserves the previous double-based API.
 *
 * Example:
 *   Threshold thresh{-1.0, 1.0};
 *   double safe = thresh.update(1.5); // safe == 1.0
 *   double raw  = thresh.current;      // raw  == 1.5
 *
 *   ThresholdT<int> intThresh{-128, 127};
 *   int safeInt = intThresh.update(200); // safeInt == 127
 *
 * @tparam T Numeric type for the threshold values.
 */
template <typename T = double>
class ThresholdT
{
public:
  /**
   * @brief Minimum allowed output value.
   */
  T min = -10. ;

  /**
   * @brief Maximum allowed output value.
   */
  T max = 10. ;

  /**
   * @brief Most recent raw input passed to update().
   */
  T current{};

  /**
   * @brief Default constructor using legacy double-compatible defaults.
   */
  ThresholdT() noexcept = default;

  /**
   * @brief Construct with explicit min/max limits.
   *
   * @param minValue Lower bound for clamping.
   * @param maxValue Upper bound for clamping.
   */
  ThresholdT(T minValue, T maxValue) noexcept(std::is_nothrow_copy_constructible_v<T>)
      : min(minValue)
      , max(maxValue)
  {
  }

  /**
   * @brief Clamp `reading` to the configured [min, max] range.
   *
   * @param reading New value to threshold.
   * @return The clamped output.
   */
  T update(T reading) noexcept(
      noexcept(current = reading) &&
      noexcept(reading < min) &&
      noexcept(reading > max))
  {
    current = reading;
    if constexpr (std::is_floating_point_v<T>) {
      if (std::isnan(reading)) {
        return min; // or max, or some other default for NaN inputs
      }
    }
    
    if (reading < min){
      return min;
    }

    if (reading > max){
      return max;
    }

    return reading;
  }
};

using Threshold = ThresholdT<double>;

}

