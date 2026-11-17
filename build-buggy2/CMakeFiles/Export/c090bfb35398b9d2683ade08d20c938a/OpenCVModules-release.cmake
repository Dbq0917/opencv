#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zlib" for configuration "Release"
set_property(TARGET zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zlib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/libzlib.a"
  )

list(APPEND _cmake_import_check_targets zlib )
list(APPEND _cmake_import_check_files_for_zlib "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/libzlib.a" )

# Import target "libjpeg-turbo" for configuration "Release"
set_property(TARGET libjpeg-turbo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "ASM_NASM;C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibjpeg-turbo.a"
  )

list(APPEND _cmake_import_check_targets libjpeg-turbo )
list(APPEND _cmake_import_check_files_for_libjpeg-turbo "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibjpeg-turbo.a" )

# Import target "libjasper" for configuration "Release"
set_property(TARGET libjasper APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjasper PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibjasper.a"
  )

list(APPEND _cmake_import_check_targets libjasper )
list(APPEND _cmake_import_check_files_for_libjasper "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibjasper.a" )

# Import target "libpng" for configuration "Release"
set_property(TARGET libpng APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libpng PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibpng.a"
  )

list(APPEND _cmake_import_check_targets libpng )
list(APPEND _cmake_import_check_files_for_libpng "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibpng.a" )

# Import target "libharfbuzz" for configuration "Release"
set_property(TARGET libharfbuzz APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libharfbuzz PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibharfbuzz.a"
  )

list(APPEND _cmake_import_check_targets libharfbuzz )
list(APPEND _cmake_import_check_files_for_libharfbuzz "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/liblibharfbuzz.a" )

# Import target "ittnotify" for configuration "Release"
set_property(TARGET ittnotify APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ittnotify PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/libittnotify.a"
  )

list(APPEND _cmake_import_check_targets ittnotify )
list(APPEND _cmake_import_check_files_for_ittnotify "${_IMPORT_PREFIX}/lib/opencv5/3rdparty/libittnotify.a" )

# Import target "opencv_core" for configuration "Release"
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_core.a"
  )

list(APPEND _cmake_import_check_targets opencv_core )
list(APPEND _cmake_import_check_files_for_opencv_core "${_IMPORT_PREFIX}/lib/libopencv_core.a" )

# Import target "opencv_flann" for configuration "Release"
set_property(TARGET opencv_flann APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_flann PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_flann.a"
  )

list(APPEND _cmake_import_check_targets opencv_flann )
list(APPEND _cmake_import_check_files_for_opencv_flann "${_IMPORT_PREFIX}/lib/libopencv_flann.a" )

# Import target "opencv_geometry" for configuration "Release"
set_property(TARGET opencv_geometry APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_geometry PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_geometry.a"
  )

list(APPEND _cmake_import_check_targets opencv_geometry )
list(APPEND _cmake_import_check_files_for_opencv_geometry "${_IMPORT_PREFIX}/lib/libopencv_geometry.a" )

# Import target "opencv_imgproc" for configuration "Release"
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_imgproc.a"
  )

list(APPEND _cmake_import_check_targets opencv_imgproc )
list(APPEND _cmake_import_check_files_for_opencv_imgproc "${_IMPORT_PREFIX}/lib/libopencv_imgproc.a" )

# Import target "opencv_photo" for configuration "Release"
set_property(TARGET opencv_photo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_photo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_photo.a"
  )

list(APPEND _cmake_import_check_targets opencv_photo )
list(APPEND _cmake_import_check_files_for_opencv_photo "${_IMPORT_PREFIX}/lib/libopencv_photo.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
