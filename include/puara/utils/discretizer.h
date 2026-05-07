/**
* @file discretizer.h
* @brief Evaluates whether a new numeric value is different from the previous one from continuous input. 
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
*/

#pragma once

#include <limits>
#include <type_traits>

namespace puara_gestures::utils
{

/**
 * @class Discretizer
 * @brief Detects when a numeric input value changes.
 *
 * @details
 * Discretizer stores the most recent input and reports whether a new value is
 * different from the previous one. It is useful for debouncing repeated identical
 * sensor or event values and only acting on actual changes.
 *
 * Example:
 * @code{.cpp}
 *   puara_gestures::utils::Discretizer<int> detector;
 *
 *   if (detector.isNew(10)) {
 *     // first value or changed value
 *   }
 *
 *   if (detector.isNew(10)) {
 *     // same value, returns false
 *   }
 *
 *   if (detector.isNew(15)) {
 *     // value changed, returns true
 *   }
 * @endcode
 *
 * @tparam T Numeric type for the input values.
 */
template <typename T = double>
class Discretizer {
private:
  static_assert(std::is_arithmetic_v<T>, "Discretizer requires an arithmetic type.");
  T latestValue;
  bool firstValue; // Flag to track if it's the first value

public:
  /**
   * @brief Construct a new Discretizer.
   *
   * @note The first value passed to `isNew()` is always considered new.
   */
  Discretizer() : latestValue(std::numeric_limits<T>::lowest()), firstValue(true) {
  }

  /**
   * @brief Update the detector and check whether the input changed.
   *
   * @param newValue The latest sampled value to compare against the previous one.
   * @return true if this is the first value or if the value differs from the last one.
   * @return false if the value is equal to the previous value.
   */
  bool isNew(const T& newValue) {
    if (firstValue) {
      latestValue = newValue;
      firstValue = false;
      return true; // It's the first value, so it's technically "different"
    }

    if (newValue != latestValue) {
      latestValue = newValue;
      return true; // Values are different
    } else {
      return false; // Values are the same
    }
  }

  /**
   * @brief Get the most recently accepted value.
   *
   * @return const T& The latest input value stored by the detector.
   */
  const T& getLatestValue() const {
    return latestValue;
  }

  /**
   * @brief Get a mutable reference to the most recently accepted value.
   *
   * @return T& The latest input value stored by the detector.
   */
  T& getLatestValue() {
    return latestValue;
  }
};

}