/**
 * @file maprange.h
 * @brief Utilities for remapping values between numeric ranges.
 * @see https://github.com/Puara/puara-gestures
 * @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
 * @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
 * @author Edu Meneses (2024) - https://www.edumeneses.com
 */
#pragma once

#include <puara/structs.h>

namespace puara_gestures::utils
{

/**
 * @class MapRange
 * @brief Simple class to remap values from one numeric range to another.
 *
 * @details
 * This helper is useful for scaling sensor values, normalizing feature inputs,
 * or converting between coordinate ranges.
 *
 *  Example usage:
 * @code
 *   puara_gestures::utils::MapRange mapper;
 *   mapper.inMin = 0;
 *   mapper.inMax = 1023;
 *   mapper.outMin = 0;
 *   mapper.outMax = 1;
 *
 *   double normalized = mapper.range(512); // ~0.5
 *   double normalized2 = mapper.range(1023); // 1.0
 *   double normalized3 = mapper.range(0); // 0.0
 * @endcode
 *
 * @note
 *   - `inMin` / `inMax` defines the source range for the input value.
 *   - `outMin` / `outMax` defines the target range for the mapped output.
 *   - If `outMin == outMax`, every input maps to the constant value `outMin`.
 *   - If `inMax == inMin`, the implementation returns `outMin` to avoid a divide-by-zero.
 *   - Reversed ranges are supported:
 *     - `inMin > inMax` inverts the direction of the input mapping.
 *     - `outMin > outMax` inverts the direction of the output mapping.
 *
 * The class also provides overloads for float and int values.
 */
class MapRange
{
public:
  /**
   * @brief Last input value that was remapped.
   */
  double current_in = 0;

  /**
   * @brief Minimum value of the input range.
   */
  double inMin = 0;

  /**
   * @brief Maximum value of the input range.
   */
  double inMax = 0;

  /**
   * @brief Minimum value of the output range.
   */
  double outMin = 0;

  /**
   * @brief Maximum value of the output range.
   */
  double outMax = 0;

  /**
   * @brief Remap a double input from the configured input range to the output range.
   *
   * @param in Input value to remap.
   * @return Mapped value in the configured output range.
   */
  double range(double in)
  {
    current_in = in;
    if (inMax == inMin) {
        return outMin;               // avoid divide by zero
    }
    if (outMin == outMax) {
        return outMin;               // constant output range
    }
    return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
  }

  /**
   * @brief Remap a float input from the configured input range to the output range.
   *
   * @param in Float input value to remap.
   * @return Mapped value in the configured output range.
   */
  float range(float in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<float>(range(casted_in));
  }

  /**
   * @brief Remap an integer input from the configured input range to the output range.
   *
   * @param in Integer input value to remap.
   * @return Mapped value in the configured output range.
   */
  int range(int in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<int>(range(casted_in));
  }
};

}
