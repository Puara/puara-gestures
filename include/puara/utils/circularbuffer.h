//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <boost/circular_buffer.hpp>
#include <puara/structs.h>

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
  boost::circular_buffer<double> buffer;

  CircularBuffer()
  {
    buffer.set_capacity(size);
  }

  double add(double element)
  {
    if (buffer.capacity() != size)
    {
      buffer.set_capacity(size);
    }
    buffer.push_front(element);
    return element;
  }
};

}
