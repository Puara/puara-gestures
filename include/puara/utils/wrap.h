//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once


#include <cmath>
#include <boost/math/constants/constants.hpp>

#ifndef M_PI
#define M_PI boost::math::constants::pi<double>()
#endif

namespace puara_gestures::utils
{
/**
 * @brief Convert wrapped angular values into a continuous angle stream.
 *
 * Unwrap is useful when you have a sequence of periodic measurements and
 * you want to recover a continuous value that keeps increasing or decreasing
 * across the wrap boundary.
 *
 * For example, if your input goes:
 * @code
 *   3.0, 3.1, -3.0, -2.9
 * @endcode
 * then the reported unwrapped values should be:
 * @code
 *   3.0, 3.1, 3.283..., 3.383...
 * @endcode
 *
 * This class uses a simple threshold-based strategy: if the jump between
 * the new reading and the previous reading is larger than half the interval,
 * it assumes a wrap crossing occurred.
 *
 * Example use-case:
 * @code
 * puara_gestures::utils::Unwrap unwrap(-M_PI, M_PI);
 * double angle = 3.0;
 * double value = unwrap.unwrap(angle);
 * angle = -3.0;
 * value = unwrap.unwrap(angle); // 3.283... instead of -3.0
 * @endcode
 */
class Unwrap
{
public:
  /**
   * @brief Previous wrapped angle value from the last `unwrap()` call.
   */
  double prev_angle{};

  /**
   * @brief Accumulated integer wrap offset.
   */
  double accum{};

  /**
   * @brief Width of the wrapped interval.
   */
  double range{};

  /**
   * @brief True when no previous value has been unwrapped yet.
   */
  bool empty{};

  /**
   * @brief Construct an Unwrap helper.
   *
   * @param Min Lower bound of the wrapped range.
   * @param Max Upper bound of the wrapped range.
   *
   * Example for typical angle data:
   * @code
   *   Unwrap u(-M_PI, M_PI);
   * @endcode
   */
  Unwrap(double Min, double Max)
      : accum(0)
      , range(Max - Min)
      , empty(true)
  {
  }

  /**
   * @brief Convert the next wrapped reading into an unwrapped value.
   *
   * @param reading New wrapped measurement.
   * @return Continuous unwrapped angle value.
   *
   * Notes:
   *   - The first call returns the raw input.
   *   - Later calls compare with the previous reading and adjust by whole-range
   *     steps when the jump crosses the boundary.
   */
  double unwrap(double reading)
  {
    double diff;

    if (empty)
    {
      diff = 0;
      empty = false;
    }
    else
    {
      diff = reading - prev_angle;
    }

    prev_angle = reading;

    if (diff > (range / 2))
    {
      accum -= 1;
    }
    if (diff < (range * -1 / 2))
    {
      accum += 1;
    }

    return reading + accum * range;
  }

  /**
   * @brief Reset the unwrapped state.
   *
   * After calling clear(), the next unwrap() call will start a new sequence.
   */
  void clear()
  {
    accum = 0;
    empty = true;
  }
};

/**
 * @brief Wrap a value into a periodic interval.
 *
 * Wrap is useful for angle normalization and other periodic ranges.
 * It maps a value into the half-open interval [min, max), where max is the
 * wrap boundary. That means `min` is included and `max` is treated as the
 * point where wrapping occurs.
 *
 * Example:
 * @code
 *   Wrap w(-M_PI, M_PI);
 *   w.wrap(3.5);      // returns approximately -2.783 (wraps above +pi)
 *   w.wrap(-3.5);     // returns approximately 2.783 (wraps below -pi)
 * @endcode
 *
 * The implementation uses `std::fmod` and normalizes negatives so that the
 * result always lies inside the interval.
 */
class Wrap
{
public:
  /**
   * @brief Inclusive lower bound of the wrapped interval.
   */
  double min{};

  /**
   * @brief Exclusive upper bound of the wrapped interval.
   */
  double max{};

  /**
   * @brief Construct a Wrap helper.
   *
   * @param minValue Inclusive lower bound of the wrapped interval.
   * @param maxValue Exclusive upper bound of the wrapped interval.
   */
  Wrap(double minValue = 0.0, double maxValue = 2.0 * M_PI)
      : min(minValue)
      , max(maxValue)
  {
  }

  /**
   * @brief Wrap a value into the interval [min, max).
   *
   * @param reading Value to normalize.
   * @return Wrapped value in [min, max).
   */
  /**
   * @brief Wrap a value into the interval [min, max).
   *
   * @param reading Value to normalize.
   * @return Wrapped value in [min, max).
   */
  double wrap(double reading) const
  {
    double range = max - min;
    if (range <= 0.0)
      return min;

    double shifted = std::fmod(reading - min, range);
    if (shifted < 0.0)
      shifted += range;

    return min + shifted;
  }
};

}

