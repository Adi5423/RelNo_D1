#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RelNo_D1::WhiteNoise" for configuration ""
set_property(TARGET RelNo_D1::WhiteNoise APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RelNo_D1::WhiteNoise PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libWhiteNoise.a"
  )

list(APPEND _cmake_import_check_targets RelNo_D1::WhiteNoise )
list(APPEND _cmake_import_check_files_for_RelNo_D1::WhiteNoise "${_IMPORT_PREFIX}/lib/libWhiteNoise.a" )

# Import target "RelNo_D1::PerlinNoise" for configuration ""
set_property(TARGET RelNo_D1::PerlinNoise APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RelNo_D1::PerlinNoise PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libPerlinNoise.a"
  )

list(APPEND _cmake_import_check_targets RelNo_D1::PerlinNoise )
list(APPEND _cmake_import_check_files_for_RelNo_D1::PerlinNoise "${_IMPORT_PREFIX}/lib/libPerlinNoise.a" )

# Import target "RelNo_D1::SimplexNoise" for configuration ""
set_property(TARGET RelNo_D1::SimplexNoise APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RelNo_D1::SimplexNoise PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libSimplexNoise.a"
  )

list(APPEND _cmake_import_check_targets RelNo_D1::SimplexNoise )
list(APPEND _cmake_import_check_files_for_RelNo_D1::SimplexNoise "${_IMPORT_PREFIX}/lib/libSimplexNoise.a" )

# Import target "RelNo_D1::PinkNoise" for configuration ""
set_property(TARGET RelNo_D1::PinkNoise APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RelNo_D1::PinkNoise PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libPinkNoise.a"
  )

list(APPEND _cmake_import_check_targets RelNo_D1::PinkNoise )
list(APPEND _cmake_import_check_files_for_RelNo_D1::PinkNoise "${_IMPORT_PREFIX}/lib/libPinkNoise.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
