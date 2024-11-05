cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(Puara-gestures-standalone
  VERSION 0.1
  DESCRIPTION "Puara-gestures over OSC"
  LANGUAGES CXX
  HOMEPAGE_URL "https://github.com/Puara/puara-gestures/standalone"
)

# Configuration
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

# Dependencies

# Enable only what is needed for this project
set(OSSIA_EDITOR 0)
set(OSSIA_DATAFLOW 0)
set(OSSIA_OVERRIDE_PROTOCOLS "OSC;OSCQuery")

FetchContent_Declare(
  libossia
  GIT_REPOSITORY "https://github.com/ossia/libossia"
  GIT_TAG master
  GIT_PROGRESS true
  GIT_SHALLOW 1
)
FetchContent_MakeAvailable(libossia)

FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        master
)

find_package(Boost REQUIRED)

set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
FetchContent_MakeAvailable(Eigen)

file(GLOB_RECURSE PUARA_SOURCE ../descriptors/*.cpp ../descriptors/*.h)

# Project setup
add_executable(puara-gestures-standalone
    main.cpp
)
target_link_libraries(puara-gestures-standalone
  PRIVATE
    ossia
    puara_gestures
)
