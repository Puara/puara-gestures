# Install script for directory: /home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lo" TYPE FILE FILES
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_cpp.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_errors.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_lowlevel.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_macros.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_osc_types.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_serverthread.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_throw.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_types.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/lo/lo_endian.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/lo/lo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7.4.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so.7.4.1"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so.7"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7.4.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/tools/oscdump")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump"
         OLD_RPATH "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/tools/oscsend")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend"
         OLD_RPATH "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo/libloTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo/libloTargets.cmake"
         "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/CMakeFiles/Export/24232cc174539220bc0fcb2f1476e63b/libloTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo/libloTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo/libloTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo" TYPE FILE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/CMakeFiles/Export/24232cc174539220bc0fcb2f1476e63b/libloTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo" TYPE FILE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/CMakeFiles/Export/24232cc174539220bc0fcb2f1476e63b/libloTargets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo" TYPE FILE FILES
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/libloConfig.cmake"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/libloConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7.4.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so.7.4.1"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so.7"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7.4.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblo.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/tools/oscdump")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump"
         OLD_RPATH "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscdump")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/tools/oscsend")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend"
         OLD_RPATH "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsend")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/tools/oscsendfile")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile"
         OLD_RPATH "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oscsendfile")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lo" TYPE FILE FILES
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_cpp.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_errors.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_lowlevel.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_macros.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_osc_types.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_serverthread.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_throw.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/liblo/cmake/../lo/lo_types.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/lo/lo_endian.h"
    "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/lo/lo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/liblo" TYPE FILE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/libloConfig.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/edu/Documents/Projetos/Puara/puara-gestures/build/liblo/cmake/liblo.pc")
endif()

