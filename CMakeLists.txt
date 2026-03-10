cmake_minimum_required(VERSION 3.24...3.30)

project(
    puara-gestures
    VERSION 0.2
    LANGUAGES CXX
)

### Library options ###
option(PUARA_GESTURES_ENABLE_TESTING "Enable building and running Puara gestures tests" ON)
option(PUARA_GESTURES_ENABLE_STANDALONE "Enable building and running Puara standalone" OFF)

### Main library ###
add_library(puara_gestures
  src/IMU_Sensor_Fusion/imu_orientation.h
  src/IMU_Sensor_Fusion/imu_orientation.cpp

  src/puara/descriptors/brushRub.h
  src/puara/descriptors/button.h
  src/puara/descriptors/jab.h
  src/puara/descriptors/roll.h
  src/puara/descriptors/shake.h
  src/puara/descriptors/simple_tilt_roll.h
  src/puara/descriptors/tilt.h
  src/puara/descriptors/touchArrayGestureDetector.h

  src/puara/utils/blobDetector.h
  src/puara/utils/calibration.h
  src/puara/utils/circularbuffer.h
  src/puara/utils/discretizer.h
  src/puara/utils/leakyintegrator.h
  src/puara/utils/maprange.h
  src/puara/utils/rollingminmax.h
  src/puara/utils/smooth.h
  src/puara/utils/threshold.h
  src/puara/utils/wrap.h

  src/puara/structs.h
  src/puara/gestures.h
  src/puara/utils.h

  src/puara_gestures.cpp

)

target_compile_definitions(puara_gestures PUBLIC $<$<BOOL:${WIN32}>:_USE_MATH_DEFINES>)
target_include_directories(puara_gestures PUBLIC src)
target_compile_features(puara_gestures PUBLIC cxx_std_20)

### Tests ###
if(PUARA_GESTURES_ENABLE_TESTING)
  include(CTest)

  add_executable(testing_roll tests/testing_roll.cpp)
  add_executable(testing_tilt tests/testing_tilt.cpp)
  add_executable(testing_touch tests/testing_touch.cpp)
  target_compile_definitions(testing_roll PRIVATE "-DPUARA_TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testing files\"")
  target_compile_definitions(testing_tilt PRIVATE "-DPUARA_TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testing files\"")
  target_compile_definitions(testing_touch PRIVATE "-DPUARA_TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testing files\"")
  target_link_libraries(testing_roll PRIVATE puara_gestures)
  target_link_libraries(testing_tilt PRIVATE puara_gestures)
  target_link_libraries(testing_touch PRIVATE puara_gestures)

endif()

### Example ###
if(PUARA_GESTURES_ENABLE_STANDALONE)
  add_subdirectory(standalone)
endif()
