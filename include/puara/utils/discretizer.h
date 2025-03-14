#pragma once

namespace puara_gestures::utils
{

template <typename T>
class Discretizer {
private:
  T latestValue;
  bool firstValue; // Flag to track if it's the first value

public:
  // Constructor
  Discretizer() : firstValue(true) {
    // Initialize latestValue to a default value.  Important to avoid
    // undefined behavior on the first comparison.  Using the minimum
    // value for numeric types is usually a good choice.
    latestValue = std::numeric_limits<T>::min(); 
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