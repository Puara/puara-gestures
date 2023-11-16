cmake_minimum_required(VERSION 3.0)

option(WITH_STATIC "Build LIBLO static library." ON)

project(puara_gestures)

add_subdirectory(liblo/cmake)

add_executable(${PROJECT_NAME} IMU_Sensor_Fusion/imu_orientation.h IMU_Sensor_Fusion/imu_orientation.cpp puara_gestures.h puara_gestures.cpp main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}/liblo/cmake 
    ${CMAKE_CURRENT_SOURCE_DIR}/liblo 
    ${CMAKE_CURRENT_SOURCE_DIR}/IMU_Sensor_Fusion 
)

target_link_libraries(${PROJECT_NAME} liblo_static )