# Install script for directory: $(SolutionDir)Source/Runtime/Utils/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.3.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "$(SolutionDir)Source/Runtime/Utils/assimp/build/lib/Debug/assimp-vc143-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "$(SolutionDir)Source/Runtime/Utils/assimp/build/lib/Release/assimp-vc143-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/anim.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/aabb.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ai_assert.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/camera.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/color4.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/color4.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/build/code/../include/assimp/config.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ColladaMetaData.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/commonMetaData.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/defs.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/cfileio.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/light.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/material.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/material.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/matrix3x3.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/matrix3x3.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/matrix4x4.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/matrix4x4.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/mesh.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ObjMaterial.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/pbrmaterial.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/GltfMaterial.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/postprocess.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/quaternion.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/quaternion.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/scene.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/metadata.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/texture.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/types.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/vector2.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/vector2.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/vector3.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/vector3.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/version.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/cimport.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/AssertHandler.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/importerdesc.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Importer.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/DefaultLogger.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ProgressHandler.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/IOStream.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/IOSystem.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Logger.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/LogStream.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/NullLogger.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/cexport.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Exporter.hpp"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/DefaultIOStream.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/DefaultIOSystem.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SceneCombiner.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/fast_atof.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/qnan.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/BaseImporter.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Hash.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ParsingUtils.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/StreamReader.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/StreamWriter.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/StringComparison.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/StringUtils.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SGSpatialSort.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/GenericProperty.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SpatialSort.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SmallVector.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SmoothingGroups.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/SmoothingGroups.inl"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/StandardShapes.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/RemoveComments.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Subdivision.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Vertex.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/LineSplitter.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/TinyFormatter.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Profiler.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/LogAux.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Bitmap.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/XMLTools.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/IOStreamBuffer.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/CreateAnimMesh.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/XmlParser.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/BlobIOSystem.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/MathFunctions.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Exceptional.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/ByteSwapper.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Compiler/poppack1.h"
    "$(SolutionDir)Source/Runtime/Utils/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "$(SolutionDir)Source/Runtime/Utils/assimp/build/code/assimp-vc143-mtd.pdb")
  endif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "$(SolutionDir)Source/Runtime/Utils/assimp/build/code/assimp-vc143-mt.pdb")
  endif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()

