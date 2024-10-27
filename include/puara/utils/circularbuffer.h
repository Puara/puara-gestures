//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>

#include <deque>

namespace puara_gestures::utils
{
/**
 *  Simple circular buffer.
 *  This was created to ensure compatibility with older ESP SoCs
 */
class CircularBuffer
{
public:
  std::size_t size = 10;
  std::deque<double> buffer;

  double add(double element)
  {
    buffer.push_front(element);
    if(buffer.size() > size)
    {
      buffer.pop_back();
    }
    return element;
  }
};

}
