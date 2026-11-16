# Install script for directory: /home/feedback/feedback/uidreczz28aHEdlribS

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "licenses" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv5" TYPE FILE RENAME "dlpack-LICENSE" FILES "/home/feedback/feedback/uidreczz28aHEdlribS/3rdparty/dlpack/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "licenses" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv5" TYPE FILE RENAME "flatbuffers-LICENSE.txt" FILES "/home/feedback/feedback/uidreczz28aHEdlribS/3rdparty/flatbuffers/LICENSE.txt")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv5/opencv2" TYPE FILE FILES "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/cvconfig.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv5/opencv2" TYPE FILE FILES "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/opencv2/opencv_modules.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5/OpenCVModules.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5/OpenCVModules.cmake"
         "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/CMakeFiles/Export/c090bfb35398b9d2683ade08d20c938a/OpenCVModules.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5/OpenCVModules-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5/OpenCVModules.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5" TYPE FILE FILES "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/CMakeFiles/Export/c090bfb35398b9d2683ade08d20c938a/OpenCVModules.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5" TYPE FILE FILES "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/CMakeFiles/Export/c090bfb35398b9d2683ade08d20c938a/OpenCVModules-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opencv5" TYPE FILE FILES
    "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/unix-install/OpenCVConfig-version.cmake"
    "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/unix-install/OpenCVConfig.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "scripts" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/CMakeFiles/install/setup_vars_opencv5.sh")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opencv5" TYPE FILE FILES
    "/home/feedback/feedback/uidreczz28aHEdlribS/platforms/scripts/valgrind.supp"
    "/home/feedback/feedback/uidreczz28aHEdlribS/platforms/scripts/valgrind_3rdparty.supp"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/3rdparty/openjpeg/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/3rdparty/harfbuzz/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/3rdparty/clapack/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/3rdparty/protobuf/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/3rdparty/ittnotify/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/include/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/calib/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/camerapipeline/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/core/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/dnn/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/features/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/flann/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/geometry/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/highgui/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/imgcodecs/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/imgproc/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/java/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/js/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/objc/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/objdetect/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/photo/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/ptcloud/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/python/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/stereo/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/stitching/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/ts/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/video/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/videoio/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/.firstpass/world/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/core/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/flann/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/geometry/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/imgproc/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/camerapipeline/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/imgcodecs/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/videoio/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/highgui/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/ts/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/doc/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/docs_sphinx/cmake_install.cmake")
  include("/home/feedback/feedback/uidreczz28aHEdlribS/build-test/apps/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
