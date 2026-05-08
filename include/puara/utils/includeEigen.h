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
#else
  #include <Eigen/Core>
  #include <Eigen/Dense>
#endif
