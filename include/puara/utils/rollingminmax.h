/**
* @file rollingminmax.h
* @brief Rolling Min/Max Tracker for Puara Gestures
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <puara/structs.h>
#include <puara/utils/circularbuffer.h>

namespace puara_gestures::utils
{

template <typename T>
/**
 * @brief Tracks the min and max values over the last N updates.
 *
 * @details RollingMinMax stores at most `buffer_size` values and recomputes the
 * minimum and maximum each time a new value is added.
 * This is useful for sliding-window feature extraction in gesture or
 * sensor processing, where you want the current range of recent values.
 *
 * Example:
 * @code{.cpp}
 *   puara_gestures::utils::RollingMinMax<int> window(3);
 *   window.update(10);
 *   window.update(4);
 *   auto range = window.update(7);  // returns MinMax<int> from puara_gestures::MinMax
 *
 *   // range.min == 4, range.max == 10
 *   // window.current_value holds the same range after the last update
 * @endcode
 *
 */
class RollingMinMax
{
public:
  explicit RollingMinMax(size_t buffer_size = 10)
      : buf(buffer_size)
  {
  }

  /**
   * @brief Latest computed minimum and maximum values.
   *
   * Always reflects the range after the most recent call to `update()`.
   */
  puara_gestures::MinMax<T> current_value;

  /**
   * @brief Add a new sample and recompute the rolling range.
   *
   * @param value New sample to include in the rolling window.
   * @return The min/max range for the current window.
   */
  puara_gestures::MinMax<T> update(T value)
  {
    puara_gestures::MinMax<T> ret{.min = value, .max = value};
    buf.add(value);
    for (const T sample : buf.buffer)
    {
      if (sample < ret.min)
        ret.min = sample;
      if (sample > ret.max)
        ret.max = sample;
    }
    current_value = ret;
    return ret;
  }

private:
  CircularBuffer<T> buf;
};

}