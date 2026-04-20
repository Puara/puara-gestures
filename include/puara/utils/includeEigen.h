// Puara Gestures - Include Eigen Header for Cross-Platform Compatibility
// This header conditionally includes the appropriate Eigen headers based on 
// the platform and available libraries. It ensures that the project can use
// Eigen for linear algebra operations while maintaining compatibility with 
// both Arduino and native C++ environments. 

#if defined(USE_ARDUINO_EIGEN)
  #include <ArduinoEigen.h>
#elif defined(USE_NATIVE_EIGEN)
  #include <Eigen/Core>
  #include <Eigen/Dense>
  #include <unsupported/Eigen/MatrixFunctions>
#elif defined(__has_include)
  #if __has_include(<ArduinoEigen.h>)
    #include <ArduinoEigen.h>
  #else
    #include <Eigen/Core>
    #include <Eigen/Dense>
    #include <unsupported/Eigen/MatrixFunctions>
  #endif
#else
  #include <Eigen/Core>
  #include <Eigen/Dense>
  #include <unsupported/Eigen/MatrixFunctions>
#endif