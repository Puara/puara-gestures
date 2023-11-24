cmake_minimum_required(VERSION 3.20)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH CMAKE_PARENT_SOURCE_DIR)

option(WITH_STATIC "Build LIBLO static library." ON)

project(puara_gestures)

add_subdirectory(liblo/cmake)

add_executable(${PROJECT_NAME} ${CMAKE_PARENT_SOURCE_DIR}/IMU_Sensor_Fusion/imu_orientation.h ${CMAKE_PARENT_SOURCE_DIR}/IMU_Sensor_Fusion/imu_orientation.cpp ${CMAKE_PARENT_SOURCE_DIR}/puara_gestures.h ${CMAKE_PARENT_SOURCE_DIR}/puara_gestures.cpp main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/liblo/cmake 
    ${CMAKE_PARENT_SOURCE_DIR}/liblo 
    ${CMAKE_PARENT_SOURCE_DIR}/IMU_Sensor_Fusion 
)

target_link_libraries(${PROJECT_NAME} liblo_static )