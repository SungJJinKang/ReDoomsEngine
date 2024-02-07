# Install script for directory: F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/include/dxc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/DXIL/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/DxilContainer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/HLSL/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/Support/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/Tracing/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dxc-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/dxc" TYPE FILE FILES
    "F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/include/dxc/config.h"
    "F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/include/dxc/dxcapi.h"
    "F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/include/dxc/dxcerrors.h"
    "F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/include/dxc/dxcisense.h"
    )
endif()

