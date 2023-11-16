#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "liblo::liblo_static" for configuration ""
set_property(TARGET liblo::liblo_static APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(liblo::liblo_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/liblo.a"
  )

list(APPEND _cmake_import_check_targets liblo::liblo_static )
list(APPEND _cmake_import_check_files_for_liblo::liblo_static "${_IMPORT_PREFIX}/lib/liblo.a" )

# Import target "liblo::liblo" for configuration ""
set_property(TARGET liblo::liblo APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(liblo::liblo PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/liblo.so.7.4.1"
  IMPORTED_SONAME_NOCONFIG "liblo.so.7"
  )

list(APPEND _cmake_import_check_targets liblo::liblo )
list(APPEND _cmake_import_check_files_for_liblo::liblo "${_IMPORT_PREFIX}/lib/liblo.so.7.4.1" )

# Import target "liblo::oscdump" for configuration ""
set_property(TARGET liblo::oscdump APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(liblo::oscdump PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/oscdump"
  )

list(APPEND _cmake_import_check_targets liblo::oscdump )
list(APPEND _cmake_import_check_files_for_liblo::oscdump "${_IMPORT_PREFIX}/bin/oscdump" )

# Import target "liblo::oscsend" for configuration ""
set_property(TARGET liblo::oscsend APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(liblo::oscsend PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/oscsend"
  )

list(APPEND _cmake_import_check_targets liblo::oscsend )
list(APPEND _cmake_import_check_files_for_liblo::oscsend "${_IMPORT_PREFIX}/bin/oscsend" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
