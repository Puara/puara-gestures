//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>

#include <list>
#include <numeric>

namespace puara_gestures::utils
{
/**
 * @brief Simple rolling average smoother for recent numeric values.
 *
 * Smooth stores the most recent values and returns their average. It is
 * intended for stabilizing noisy sensor readings in a small moving window.
 *
 * Example:
 *   Smooth smoother(5.0);
 *   double filtered = smoother.smooth(rawValue);
 *
 * The public field `size` defines how many values are kept. When the window
 * is full, the oldest reading is dropped as a new value arrives.
 */
class Smooth
{
public:
  /**
   * @brief History of the most recent values.
   */
  std::list<double> list;

  /**
   * @brief Number of recent values to average.
   */
  std::size_t size = 0;

  /**
   * @brief Constructor for Smooth.
   *
   * @param Size Number of previous values to include in the average.
   */
  explicit Smooth(std::size_t Size = 50)
      : size(Size)
      , sum(0.0)
  {
  }

  /**
   * @brief Smooth a new reading and return the rolling average.
   */
  double smooth(double reading)
  {
    updateList(reading);
    if (list.empty())
    {
      return 0.0;
    }
    return (sum / static_cast<double>(list.size()));
  }

  /**
   * @brief Update the rolling window with a new value.
   */
  void updateList(double reading)
  {
    list.push_front(reading);
    sum += reading;

    while (list.size() > static_cast<std::size_t>(size))
    {
      sum -= list.back();
      list.pop_back();
    }
  }

  /**
   * @brief Clear the stored history and reset the sum.
   */
  void clear()
  {
    list.clear();
    sum = 0.0;
  }

private:
  double sum = 0.0;
};

}
