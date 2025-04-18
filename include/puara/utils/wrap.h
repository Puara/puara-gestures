//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <cmath>

namespace puara_gestures::utils
{
/**
 * @brief Wrapper class undoes modular effects of angle measurements
 * Ensures that instead of "wrapping" (e.g. moving from - PI to PI),
 * measurements continue to decrease or increase
 */
class Unwrap
{
public:
  double prev_angle{};
  double accum{};
  double range{};
  bool empty{};

  /**
   * @brief Constructor for Unwrap class
   *
   * Initaliazes accumulated at 0 to indicate the data has not "wrapped" yet
   * Initalizes empty at True
   * @param Min minimum input value
   * @param Max maximum input value
   */
  Unwrap(double Min, double Max)
      : accum(0)
      , range(Max - Min)
      , empty(true)
  {
  }

  /**
   * @brief Calculates whether angle has "wrapped" based on previous angle
   * and updates accordingly
   */
  double unwrap(double reading)
  {
    double diff;

    // check if update hasn't been called before or the unwrap class
    // has been cleared
    if(empty)
    {
      diff = 0;
      empty = false;
    }
    else
    {
      diff = reading - prev_angle;
    }
    prev_angle = reading;
    if(diff > (range / 2))
    {
      accum -= 1;
    }
    if(diff < (range * -1 / 2))
    {
      accum += 1;
    }
    return reading + accum * range;
  }

  /**
   * @brief resets the unwrap object
   */
  void clear()
  {
    accum = 0;
    empty = true;
  }
};

/**
 * @brief Wraps a value around a given minimum and maximum.
 * Algorithm from Jean-Michael Celerier. "Authoring interactive media : a logical
 * & temporal approach." Computation and Language [cs.CL]. Université de Bordeaux,
 * 2018. English. ffNNT : 2018BORD0037ff. fftel-01947309
 */
class Wrap
{
public:
  double min{};
  double max{};

  double wrap(double reading)
  {
    double diff = std::fabs(max - min);
    if(min <= reading && reading <= max)
    {
      return reading;
    }
    else if(reading >= max)
    {
      return min + (reading - std::fmod(min, diff));
    }
    return max - (min - std::fmod(reading, diff));
  }
};

}
