# Install script for directory: $(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/clang/unittests

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/install/x64-Debug")
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
  # Include the install script for each subdirectory.
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/Basic/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/Lex/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/Rewrite/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/CodeGen/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/HLSL/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/HLSLTestLib/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/HLSLErrors/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/HLSLExec/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/HLSLHost/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/dxc_batch/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/DxrFallback/cmake_install.cmake")
  include("$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/tools/clang/unittests/SPIRV/cmake_install.cmake")

endif()

