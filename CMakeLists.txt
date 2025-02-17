cmake_minimum_required(VERSION 3.24...3.30)

project(
    puara-gestures
    VERSION 0.2
    LANGUAGES CXX
)

### Library options ###
option(PUARA_GESTURES_ENABLE_TESTING "Enable building and running Puara gestures tests" ON)
option(PUARA_GESTURES_ENABLE_STANDALONE "Enable building and running Puara standalone" OFF)

### Dependencies ###
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/3rdparty")
set(Boost_NO_SYSTEM_PATHS ON)
find_package(Boost REQUIRED)

### Main library ###
add_library(puara_gestures
  3rdparty/IMU_Sensor_Fusion/imu_orientation.h
  3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp

  include/puara/descriptors/button.h
  include/puara/descriptors/jab.h
  include/puara/descriptors/roll.h
  include/puara/descriptors/shake.h
  include/puara/descriptors/tilt.h
  include/puara/descriptors/touchArrayGestureDetector.h

  include/puara/utils/blobDetector.h
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

)

target_compile_definitions(puara_gestures PUBLIC $<$<BOOL:${WIN32}>:_USE_MATH_DEFINES>)
target_include_directories(puara_gestures PUBLIC include 3rdparty)
target_compile_features(puara_gestures PUBLIC cxx_std_20)
target_link_libraries(puara_gestures PUBLIC Boost::headers)

### Tests ###
if(PUARA_GESTURES_ENABLE_TESTING)
  include(CTest)

target_sources(Continuous
  PRIVATE
  tests/testing_touch.cpp

)
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
