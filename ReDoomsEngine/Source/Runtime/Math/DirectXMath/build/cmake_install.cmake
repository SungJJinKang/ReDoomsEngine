# Install script for directory: $(SolutionDir)Source/Runtime/Math/DirectXMath

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/DirectXMath")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake"
         "$(SolutionDir)Source/Runtime/Math/DirectXMath/build/CMakeFiles/Export/share/directxmath/DirectXMath-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmath" TYPE FILE FILES "$(SolutionDir)Source/Runtime/Math/DirectXMath/build/CMakeFiles/Export/share/directxmath/DirectXMath-targets.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/directxmath" TYPE FILE FILES
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXCollision.h"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXCollision.inl"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXColors.h"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXMath.h"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXMathConvert.inl"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXMathMatrix.inl"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXMathMisc.inl"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXMathVector.inl"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXPackedVector.h"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/Inc/DirectXPackedVector.inl"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmath" TYPE FILE FILES
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/build/directxmath-config.cmake"
    "$(SolutionDir)Source/Runtime/Math/DirectXMath/build/directxmath-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "$(SolutionDir)Source/Runtime/Math/DirectXMath/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
