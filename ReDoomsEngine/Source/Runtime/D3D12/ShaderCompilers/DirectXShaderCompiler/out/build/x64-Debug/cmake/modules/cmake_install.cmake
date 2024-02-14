# Install script for directory: $(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/cmake/modules

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/llvm/cmake/LLVMExports.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/llvm/cmake/LLVMExports.cmake"
         "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/cmake/modules/CMakeFiles/Export/132b3499cd24fa5a036a97931d7c31f0/LLVMExports.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/llvm/cmake/LLVMExports-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/llvm/cmake/LLVMExports.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/llvm/cmake" TYPE FILE FILES "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/cmake/modules/CMakeFiles/Export/132b3499cd24fa5a036a97931d7c31f0/LLVMExports.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/llvm/cmake" TYPE FILE FILES "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/cmake/modules/CMakeFiles/Export/132b3499cd24fa5a036a97931d7c31f0/LLVMExports-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/llvm/cmake" TYPE FILE FILES
    "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/cmake/modules/CMakeFiles/LLVMConfig.cmake"
    "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/share/llvm/cmake/LLVMConfigVersion.cmake"
    "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/cmake/modules/LLVM-Config.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/llvm/cmake" TYPE DIRECTORY FILES "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/cmake/modules/." FILES_MATCHING REGEX "/[^/]*\\.cmake$" REGEX "/\\.svn$" EXCLUDE REGEX "/llvmconfig\\.cmake$" EXCLUDE REGEX "/llvmconfigversion\\.cmake$" EXCLUDE REGEX "/llvm\\-config\\.cmake$" EXCLUDE REGEX "/gethosttriple\\.cmake$" EXCLUDE REGEX "/versionfromvcs\\.cmake$" EXCLUDE REGEX "/checkatomic\\.cmake$" EXCLUDE)
endif()

