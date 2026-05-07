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
 *  @brief Simple class to remap values from one numeric range to another.
 *
 *  This helper is useful for scaling sensor values, normalizing feature inputs,
 *  or converting between coordinate ranges.
 *
 *  Example usage:
 *    MapRange mapper;
 *    mapper.inMin = 0;
 *    mapper.inMax = 1023;
 *    mapper.outMin = 0;
 *    mapper.outMax = 1;
 *
 *    double normalized = mapper.range(512); // ~0.5
 *
 *  Notes:
 *    - `inMin` / `inMax` defines the input range.
 *    - `outMin` / `outMax` defines the output range.
 *    - If `outMin == outMax`, the output becomes constant at `outMin`.
 *    - If `inMax == inMin`, the implementation returns `outMin` to avoid a divide-by-zero.
 *    - Reversed ranges are supported: `inMin > inMax` inverts the input mapping, and
 *      `outMin > outMax` inverts the output direction.
 *
 *  The class also provides overloads for float and int values.
 */
class MapRange
{
public:
  double current_in = 0;
  double inMin = 0;
  double inMax = 0;
  double outMin = 0;
  double outMax = 0;

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

  float range(float in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<float>(range(casted_in));
  }

  int range(int in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<int>(range(casted_in));
  }
};

}
