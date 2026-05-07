// Puara Gestures - Include Eigen Header for Cross-Platform Compatibility
// This header conditionally includes the appropriate Eigen headers based on 
// the platform and available libraries. It ensures that the project can use
// Eigen for linear algebra operations while maintaining compatibility with 
// both Arduino and native C++ environments. 
#pragma once


#if __has_include(<ArduinoEigen.h>)
  #include <ArduinoEigen.h>
#else
  #include <Eigen/Core>
  #include <Eigen/Dense>
#endif
