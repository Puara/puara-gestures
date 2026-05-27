/**
 * @file includeEigen.h
 * @brief Include Eigen headers for Arduino or native C++ environments.
 *
 * @details
 * This header conditionally includes `ArduinoEigen.h` when available,
 * otherwise it falls back to standard Eigen headers.
 */
#pragma once

#if __has_include(<ArduinoEigen.h>)
  #include <ArduinoEigen.h>
  #define PUARA_HAS_EIGEN 1 
#elif __has_include(<Eigen/Core>) && __has_include(<Eigen/Dense>)
  #include <Eigen/Core>
  #include <Eigen/Dense>
  #define PUARA_HAS_EIGEN 1
#endif
