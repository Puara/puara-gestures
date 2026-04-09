//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>

namespace puara_gestures::utils
{

/**
 * @brief Clamp a numeric value to a configurable range.
 *
 * Threshold keeps a minimum and maximum bound and prevents values from
 * exceeding those limits. The most recent raw input is also stored in
 * `current` so users can inspect the last sample that was evaluated.
 *
 * Example:
 *   Threshold thresh;
 *   thresh.min = -1.0;
 *   thresh.max =  1.0;
 *
 *   double safe = thresh.update(1.5); // safe == 1.0
 *   double raw  = thresh.current;      // raw  == 1.5
 */
class Threshold
{
public:
  /**
   * @brief Minimum allowed output value.
   */
  double min{-10.0};

  /**
   * @brief Maximum allowed output value.
   */
  double max{10.0};

  /**
   * @brief Most recent raw input passed to update().
   */
  double current{};

  /**
   * @brief Clamp `reading` to the configured [min, max] range.
   *
   * @param reading New value to threshold.
   * @return The clamped output.
   */
  double update(double reading)
  {
    current = reading;
    if (reading < min)
    {
      return min;
    }
    if (reading > max)
    {
      return max;
    }
    return reading;
  }
};

}
