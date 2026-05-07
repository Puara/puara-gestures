/**
* @file circularbuffer.h
* @brief Simple circular buffer implementation.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/

#pragma once

#include <boost/circular_buffer.hpp>
#include <puara/structs.h>

namespace puara_gestures::utils
{
/**
 * @class CircularBuffer
 * @brief Simple circular buffer wrapper for fixed-size element storage.
 *
 * @details
 * CircularBuffer stores a fixed number of values and automatically drops the
 * oldest element when new values are added beyond capacity. It is intended for
 * lightweight buffering on platforms where the Boost circular buffer is used
 * as the underlying container.
 *
 * Example:
 * @code{.cpp}
 *   // default stores doubles
 *   puara_gestures::utils::CircularBuffer<> buffer;
 *   buffer.add(1.0);
 *   buffer.add(2.0);
 *
 *   // templated for other types
 *   puara_gestures::utils::CircularBuffer<int> intBuffer(4);
 *   intBuffer.add(10);
 *   intBuffer.add(20);
 * @endcode
 *
 * @tparam T Value type stored in the buffer.
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
  boost::circular_buffer<T> buffer = boost::circular_buffer<T>(size);

  CircularBuffer(const CircularBuffer&) = default;
  CircularBuffer(CircularBuffer&&) noexcept = default;
  CircularBuffer& operator=(const CircularBuffer&) = default;
  CircularBuffer& operator=(CircularBuffer&&) noexcept = default;

  /**
   * @brief Construct a circular buffer with the default capacity.
   */
  CircularBuffer() : buffer(size) {}

  /**
   * @brief Construct a circular buffer with a custom capacity.
   *
   * @param capacity Number of elements to retain in the buffer.
   */
  explicit CircularBuffer(std::size_t capacity) : size(capacity), buffer(capacity) {}

  /**
   * @brief Add an element to the circular buffer.
   *
   * @param element The value to push into the buffer.
   * @return T The same element that was added.
   */
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
