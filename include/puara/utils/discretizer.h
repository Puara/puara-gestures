#pragma once

#ifndef DISCRETIZER_H
#define DISCRETIZER_H

#include <limits>
#include <type_traits>

namespace puara_gestures::utils
{

/**
 * @brief Simple change detector for numeric values.
 *
 * This helper remembers the most recent input and reports whether a new
 * value is different from the previous one. It does not keep a history of
 * past values, only the current state.
 *
 * Example:
 *   Discretizer<int> detector;
 *   if (detector.isNew(reading)) {
 *     // first reading or changed value
 *   }
 *
 * This is useful when you want to ignore repeated identical sensor or event
 * readings, and only act when the value changes.
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
  // Constructor
  Discretizer() : latestValue(std::numeric_limits<T>::lowest()), firstValue(true) {
  }

  // Method to update the value and check for difference
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

  // Method to get the latest value (const version)
  const T& getLatestValue() const {
    return latestValue;
  }

  // Method to get the latest value (non-const version)
  T& getLatestValue() {
    return latestValue;
  }
};

}
#endif // DISCRETIZER_H