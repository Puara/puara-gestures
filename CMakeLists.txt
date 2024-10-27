cmake_minimum_required(VERSION 3.24...3.30)

project(
    puara-gestures
    VERSION 0.2
    LANGUAGES CXX
)

### Library options ###
option(PUARA_GESTURES_ENABLE_TESTING "Enable building and running Puara gestures tests" ON)

### Main library ###
add_library(puara_gestures
  3rdparty/IMU_Sensor_Fusion/imu_orientation.h
  3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp

  include/puara/descriptors/button.h
  include/puara/descriptors/jab.h
  include/puara/descriptors/roll.h
  include/puara/descriptors/shake.h
  include/puara/descriptors/tilt.h
  include/puara/descriptors/touch.h

  include/puara/utils/calibration.h
  include/puara/utils/circularbuffer.h
  include/puara/utils/leakyintegrator.h
  include/puara/utils/maprange.h
  include/puara/utils/rollingminmax.h
  include/puara/utils/smooth.h
  include/puara/utils/threshold.h
  include/puara/utils/wrap.h

  include/puara/structs.h
  include/puara/gestures.h
  include/puara/utils.h

  src/puara_gestures.cpp
)

target_include_directories(puara_gestures PUBLIC include 3rdparty)

### Tests ###
if(PUARA_GESTURES_ENABLE_TESTING)
  include(CTest)
  add_executable(testing_roll tests/testing_roll.cpp)
  add_executable(testing_tilt tests/testing_tilt.cpp)
  target_compile_definitions(testing_roll PRIVATE "-DPUARA_TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testing files\"")
  target_compile_definitions(testing_tilt PRIVATE "-DPUARA_TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testing files\"")
  target_link_libraries(testing_roll PRIVATE puara_gestures)
  target_link_libraries(testing_tilt PRIVATE puara_gestures)
endif()
