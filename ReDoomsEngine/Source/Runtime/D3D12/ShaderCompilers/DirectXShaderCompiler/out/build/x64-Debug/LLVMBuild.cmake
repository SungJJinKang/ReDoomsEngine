#===-- LLVMBuild.cmake - LLVMBuild Configuration for LLVM -----*- CMake -*--===#
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#
#
# This file contains the LLVMBuild project information in a format easily
# consumed by the CMake based build system.
#
# This file is autogenerated by llvm-build, do not edit!
#
#===------------------------------------------------------------------------===#


# LLVMBuild CMake fragment dependencies.
#
# CMake has no builtin way to declare that the configuration depends on
# a particular file. However, a side effect of configure_file is to add
# said input file to CMake's internal dependency list. So, we use that
# and a dummy output file to communicate the dependency information to
# CMake.
#
# FIXME: File a CMake RFE to get a properly supported version of this
# feature.

if(NOT HLSL_OFFICIAL_BUILD)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/docs/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/examples/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Analysis/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Analysis/IPA/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/AsmParser/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Bitcode/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Bitcode/Reader/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Bitcode/Writer/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/CodeGen/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/CodeGen/AsmPrinter/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/CodeGen/SelectionDAG/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/CodeGen/MIRParser/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DebugInfo/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DebugInfo/DWARF/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DebugInfo/PDB/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxcSupport/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilPIXPasses/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/Interpreter/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/MCJIT/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/RuntimeDyld/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/IntelJITEvents/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/OProfileJIT/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ExecutionEngine/Orc/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Linker/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/IR/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/IRReader/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/MC/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/MC/MCDisassembler/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/MC/MCParser/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Object/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Option/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Passes/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/PassPrinters/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/ProfileData/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Support/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/TableGen/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Target/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/IPO/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/InstCombine/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/Instrumentation/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/Scalar/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/Utils/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/Vectorize/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/Transforms/ObjCARC/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/HLSL/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DXIL/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilContainer/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilPdbInfo/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilDia/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxrFallback/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilRootSignature/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxcBindingTable/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/lib/DxilCompression/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/projects/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/dsymutil/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llc/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/lli/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/lli/ChildTarget/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-as/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-bcanalyzer/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-cov/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-diff/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-dis/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-dwarfdump/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-extract/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-link/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-mc/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-mcmarkup/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-nm/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-objdump/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-pdbdump/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-profdata/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-rtdyld/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/llvm-size/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/macho-dump/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/opt/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/tools/verify-uselistorder/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/TableGen/LLVMBuild.txt"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvm-build"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvmbuild/util.py"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvmbuild/componentinfo.py"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvmbuild/configutil.py"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvmbuild/main.py"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)
configure_file("F:/ReDoomsEngine/ReDoomsEngine/Source/Runtime/D3D12/ShaderCompilers/DirectXShaderCompiler/utils/llvm-build/llvmbuild/__init__.py"
               ${CMAKE_CURRENT_BINARY_DIR}/DummyConfigureOutput)

endif(NOT HLSL_OFFICIAL_BUILD)

# Explicit library dependency information.
#
# The following property assignments effectively create a map from component
# names to required libraries, in a way that is easily accessed from CMake.
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMSupport )
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMBitReader LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxcSupport )
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDXIL LLVMBitReader LLVMCore LLVMDxcSupport LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAnalysis LLVMCore LLVMDXIL LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAsmParser LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMipa LLVMAnalysis LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTransformUtils LLVMAnalysis LLVMCore LLVMSupport LLVMipa)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMInstCombine LLVMAnalysis LLVMCore LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMProfileData LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMScalarOpts LLVMAnalysis LLVMCore LLVMInstCombine LLVMProfileData LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTarget LLVMAnalysis LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMCodeGen LLVMAnalysis LLVMCore LLVMScalarOpts LLVMSupport LLVMTarget LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAsmPrinter LLVMAnalysis LLVMCodeGen LLVMCore LLVMSupport LLVMTarget LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMBitWriter LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMObject LLVMBitReader LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDebugInfoDWARF LLVMObject LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDebugInfoPDB LLVMObject LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxcBindingTable LLVMCore LLVMDXIL LLVMDxcSupport LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilCompression )
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilContainer LLVMBitReader LLVMBitWriter LLVMCore LLVMDXIL LLVMDxcSupport LLVMSupport LLVMTransformUtils LLVMipa)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilPIXPasses LLVMBitReader LLVMCore LLVMDxcSupport LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilDia LLVMCore LLVMDxcSupport LLVMDxilPIXPasses LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilPdbInfo LLVMCore LLVMDxcSupport LLVMDxilContainer LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxilRootSignature LLVMBitReader LLVMCore LLVMDXIL LLVMDxcSupport LLVMDxilContainer LLVMSupport LLVMipa)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDxrFallback LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMC LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMRuntimeDyld LLVMMC LLVMObject LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMExecutionEngine LLVMCore LLVMMC LLVMObject LLVMRuntimeDyld LLVMSupport LLVMTarget)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMInterpreter LLVMCodeGen LLVMCore LLVMExecutionEngine LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMHLSL LLVMBitReader LLVMCore LLVMDXIL LLVMDxcBindingTable LLVMDxcSupport LLVMDxilContainer LLVMSupport LLVMipa)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMVectorize LLVMAnalysis LLVMCore LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMipo LLVMAnalysis LLVMCore LLVMInstCombine LLVMScalarOpts LLVMSupport LLVMTransformUtils LLVMVectorize LLVMipa)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMIRReader LLVMAsmParser LLVMBitReader LLVMCore LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMInstrumentation LLVMAnalysis LLVMCore LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMLinker LLVMCore LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMCDisassembler LLVMMC LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMCJIT LLVMCore LLVMExecutionEngine LLVMObject LLVMRuntimeDyld LLVMSupport LLVMTarget)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMCParser LLVMMC LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMIRParser LLVMAsmParser LLVMCodeGen LLVMCore LLVMSupport LLVMTarget)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMObjCARCOpts LLVMAnalysis LLVMCore LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMOption LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMOrcJIT LLVMCore LLVMExecutionEngine LLVMObject LLVMRuntimeDyld LLVMSupport LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMPasses LLVMAnalysis LLVMCore LLVMInstCombine LLVMScalarOpts LLVMSupport LLVMTransformUtils LLVMVectorize LLVMipa LLVMipo)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMPassPrinters LLVMAnalysis LLVMCore LLVMInstCombine LLVMPasses LLVMScalarOpts LLVMSupport LLVMTransformUtils LLVMVectorize LLVMipa LLVMipo)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMSelectionDAG LLVMAnalysis LLVMCodeGen LLVMCore LLVMSupport LLVMTarget LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTableGen LLVMSupport)