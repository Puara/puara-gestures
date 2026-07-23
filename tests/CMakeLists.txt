cmake_minimum_required(VERSION 3.24)
project(puara_gestures_tests LANGUAGES CXX)


Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)



# Validate the executables return success.

enable_testing()

# Common include paths for both tests.
set(TEST_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/eigen
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# One translation unit per descriptor keeps these files small as the
# descriptor library grows. Add a new descriptor's test as its own
# descriptors/<name>.cpp entry here.
add_executable(test_descriptors
 descriptors/roll.cpp
 descriptors/tilt.cpp
 descriptors/simple_tilt_roll.cpp
 descriptors/jab.cpp
 descriptors/shake.cpp
 descriptors/touch_array.cpp
 descriptors/button.cpp
 ../3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp
)
target_compile_features(test_descriptors PRIVATE cxx_std_20)
target_link_libraries(test_descriptors PRIVATE Catch2::Catch2WithMain)
target_include_directories(test_descriptors PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME test_descriptors COMMAND test_descriptors)
set_tests_properties(test_descriptors PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

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

add_executable(imu-filters
 testing_IMU-filters.cpp
)
target_compile_features(imu-filters PRIVATE cxx_std_20)
target_link_libraries(imu-filters PRIVATE Catch2::Catch2WithMain)
target_include_directories(imu-filters PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME imu-filters COMMAND imu-filters)
set_tests_properties(imu-filters PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(magnetometerCalibration
  testing_magnetometerCalibration.cpp
)
target_compile_features(magnetometerCalibration PRIVATE cxx_std_20)
target_link_libraries(magnetometerCalibration PRIVATE Catch2::Catch2WithMain)
target_include_directories(magnetometerCalibration PRIVATE ${TEST_INCLUDE_DIRS})
add_test(NAME magnetometerCalibration COMMAND magnetometerCalibration)
set_tests_properties(magnetometerCalibration PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
