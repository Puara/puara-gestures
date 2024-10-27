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
 * Simple Threshold class to ensure a value doesn't exceed settable max and min values.
 */
class Threshold
{
public:
  double min;
  double max;
  double current;

  Threshold(double Min = -10.0, double Max = 10.0)
      : min(Min)
      , max(Max)
  {
  }

  double update(double reading)
  {
    current = reading;
    if(reading < min)
    {
      return min;
    }
    if(reading > max)
    {
      return max;
    }
    return reading;
  }
};

}
