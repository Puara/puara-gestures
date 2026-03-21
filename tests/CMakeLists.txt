cmake_minimum_required(VERSION 3.24)
project(puara_gestures_tests LANGUAGES CXX)


Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)



# Validate the executables return success.

enable_testing()

# Common include paths for both tests.
set(TEST_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(roll
  testing_roll.cpp
  ../3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp
)
target_compile_features(roll PRIVATE cxx_std_20)
target_include_directories(roll PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME roll COMMAND roll)
set_tests_properties(roll PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(tilt
  testing_tilt.cpp
  ../3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp
)
target_compile_features(tilt PRIVATE cxx_std_20)
target_include_directories(tilt PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME tilt COMMAND tilt)
set_tests_properties(tilt PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(touch
  testing_touch.cpp
  ../3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp
)
target_compile_features(touch PRIVATE cxx_std_20)
target_include_directories(touch PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME touch COMMAND touch)
set_tests_properties(touch PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(utils
  test_utils.cpp
)
target_compile_features(utils PRIVATE cxx_std_20)
target_link_libraries(utils PRIVATE Catch2::Catch2WithMain)
target_include_directories(utils PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME utils COMMAND utils)
set_tests_properties(utils PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
