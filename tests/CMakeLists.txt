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

add_executable(structs
  testing_structs.cpp
)
target_compile_features(structs PRIVATE cxx_std_20)
target_link_libraries(structs PRIVATE Catch2::Catch2WithMain)
target_include_directories(structs PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME structs COMMAND structs)
set_tests_properties(structs PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(leakyintegrator
  testing_leakyintegrator.cpp
)
target_compile_features(leakyintegrator PRIVATE cxx_std_20)
target_link_libraries(leakyintegrator PRIVATE Catch2::Catch2WithMain)
target_include_directories(leakyintegrator PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME leakyintegrator COMMAND leakyintegrator)
set_tests_properties(leakyintegrator PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(blobDetector
  testing_blobDetector.cpp
)
target_compile_features(blobDetector PRIVATE cxx_std_20)
target_link_libraries(blobDetector PRIVATE Catch2::Catch2WithMain)
target_include_directories(blobDetector PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME blobDetector COMMAND blobDetector)
set_tests_properties(blobDetector PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(magnetometerCalibration
  testing_magnetometerCalibration.cpp
)
target_compile_features(magnetometerCalibration PRIVATE cxx_std_20)
target_link_libraries(magnetometerCalibration PRIVATE Catch2::Catch2WithMain)
target_include_directories(magnetometerCalibration PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME magnetometerCalibration COMMAND magnetometerCalibration)
set_tests_properties(magnetometerCalibration PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
