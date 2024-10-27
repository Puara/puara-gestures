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
  * @brief "Smoothing" algorithm takes the average of a given number of previous
  * inputs. Can stabilize an erratic input stream.
  */
class Smooth
{
public:
  std::list<double> list;
  double size;

  /**
   * @brief Constructor for Smooth class
   *
   * @param Size number of previous values that "smoother" object averages
   */
  explicit Smooth(double Size = 50.)
      : size(Size)
  {
  }

  /**
   * @brief Calls updateList then finds average of previous inputs
   */
  double smooth(double reading)
  {
    updateList(reading);
    double sumList = std::accumulate(list.begin(), list.end(), 0.0);
    return (sumList / list.size());
  }

  /**
  * @brief Updates list of previous inputs with current input
  */
  void updateList(double reading)
  {
    list.push_front(reading);
    // ensure list stays at desired size
    while(list.size() > size)
    {
      list.pop_back();
    }
  }

  /**
   * @brief Clears list of all previous inputs
   */
  void clear() { list.clear(); }
};

}
