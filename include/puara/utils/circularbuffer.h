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
 *  This was created to ensure compatibility with older ESP SoCs.
 *
 *  Usage:
 *    // default stores doubles
 *    CircularBuffer<> buffer;
 *    buffer.add(1.0);
 *    buffer.add(2.0);
 *
 *    // templated for other types
 *    CircularBuffer<int> intBuffer(4);
 *    intBuffer.add(10);
 *    intBuffer.add(20);
 *
 *  The buffer keeps a fixed capacity and automatically drops the oldest
 *  values once capacity is exceeded.
 *
 *  Notes:
 *    - The internal `buffer` is a `boost::circular_buffer<T>`.
 *    - Accessing `buffer` out of range with `operator[]` is unchecked and
 *      is the caller's responsibility.
 *    - Use `buffer.at(index)` if you need bounds-checked access.
 */
template <typename T = double>
class CircularBuffer
{
public:
  /** 
   * @brief Requested capacity for the circular buffer. 
   * */
  std::size_t size = 10;

  /** 
   * @brief Underlying storage for circular buffer elements. 
   * */
  boost::circular_buffer<T> buffer;

  CircularBuffer(const CircularBuffer&) = default;
  CircularBuffer(CircularBuffer&&) noexcept = default;
  CircularBuffer& operator=(const CircularBuffer&) = default;
  CircularBuffer& operator=(CircularBuffer&&) noexcept = default;

  CircularBuffer() : buffer(size) {}

  explicit CircularBuffer(std::size_t capacity) : size(capacity), buffer(capacity) {}

  T add(const T& element)
  {
    if (buffer.capacity() != size){
      buffer.set_capacity(size);
    }
    buffer.push_front(element);
    return element;
  }
};

}
