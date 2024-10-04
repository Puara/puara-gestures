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
  GIT_REPOSITORY "https://github.com/OSSIA/libossia"
  GIT_TAG master
  GIT_PROGRESS true
)
FetchContent_MakeAvailable(libossia)

file(GLOB_RECURSE PUARA_SOURCE ../descriptors/*.cpp)

add_library(puara-gestures ${PUARA_SOURCE})

# Project setup
add_executable(puara-gestures-standalone)

target_sources(puara-gestures-standalone 
  PRIVATE 
  main.cpp
)
target_link_libraries(puara-gestures-standalone
  PRIVATE 
  ossia
  puara-gestures
)
target_include_directories(puara-gestures-standalone 
  PRIVATE 
  ${libossia_SOURCE_DIR}/ossia/include
  ${libossia_SOURCE_DIR}/ossia
)
