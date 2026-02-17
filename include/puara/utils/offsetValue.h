//********************************************************************************//
// Puara Gestures - Projection (.h)                                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Zachary L'Heureux (2025)                                                       //
//********************************************************************************//

#pragma once

namespace puara_gestures::utils
{

/**
 * @brief Simple class to offset values while looping back to a range of values.
 *        This is useful for values that are not continuous, like angles.
 */
class OffsetValue
{
public:
  double current_in = 0;
  double offsetAmount = 0;
  double minValue = 0;
  double maxValue = 0;

  double offset(double in)
  {
    current_in = in;
    current_in -= offsetAmount; // Subtract the offset amount first
    // Wrap around the value to stay within the range
    if(current_in < minValue)
    {
      current_in = maxValue - (minValue - current_in);
    }
    if(current_in > maxValue)
    {
      current_in = minValue + (current_in - maxValue);
    }
    return current_in;
  }

  float offset(float in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<float>(offset(casted_in));
  }

  int offset(int in)
  {
    double casted_in = static_cast<double>(in);
    return static_cast<int>(offset(casted_in));
  }
};
}
