//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>
#include <puara/utils/circularbuffer.h>

namespace puara_gestures::utils
{

template <typename T>
/**
 * @brief Calculates the minimum and maximum values of the last N updates.
 * The default N value is 10, modifiable during initialization.
 * Ported from https://github.com/celtera/avendish/blob/56b89e52e367c67213be0c313d2ed3b9fb1aac19/examples/Ports/Puara/Jab.hpp#L15
 */
class RollingMinMax
{
public:
  RollingMinMax(size_t buffer_size = 10)
  {
    buf.size = buffer_size;
  }

  puara_gestures::MinMax<T> current_value;
  puara_gestures::MinMax<T> update(T value)
  {
    puara_gestures::MinMax<T> ret{.min = value, .max = value};
    buf.add(value);
    for(const T& v : buf.buffer)
    {
      if(v < ret.min)
        ret.min = v;
      if(v > ret.max)
        ret.max = v;
    }
    current_value = ret;
    return ret;
  }

private:
  CircularBuffer<T> buf;
};

}
