# CMake generated Testfile for 
# Source directory: $(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools
# Build directory: $(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/out/build/x64-Debug/external/SPIRV-Tools/test/tools
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(spirv-tools_expect_unittests "C:/Users/hour30000/AppData/Local/Programs/Python/Python312/python.exe" "-m" "unittest" "expect_unittest.py")
set_tests_properties(spirv-tools_expect_unittests PROPERTIES  WORKING_DIRECTORY "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools" _BACKTRACE_TRIPLES "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools/CMakeLists.txt;15;add_test;$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools/CMakeLists.txt;0;")
add_test(spirv-tools_spirv_test_framework_unittests "C:/Users/hour30000/AppData/Local/Programs/Python/Python312/python.exe" "-m" "unittest" "spirv_test_framework_unittest.py")
set_tests_properties(spirv-tools_spirv_test_framework_unittests PROPERTIES  WORKING_DIRECTORY "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools" _BACKTRACE_TRIPLES "$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools/CMakeLists.txt;18;add_test;$(SolutionDir)Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/external/SPIRV-Tools/test/tools/CMakeLists.txt;0;")
subdirs("opt")
subdirs("objdump")
