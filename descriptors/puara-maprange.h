//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include "puara-structs.h"

namespace puara_gestures::utils
{

/**
 *  @brief Simple class to renge values according to min and max (in and out)
 *  established values.
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
    if(outMin != outMax)
    {
      return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    else
    {
      return in;
    }
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
