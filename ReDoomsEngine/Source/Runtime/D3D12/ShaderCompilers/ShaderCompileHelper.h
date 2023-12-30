#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "ShaderCompileStructs.h"

struct IDxcLibrary;
struct IDxcUtils;
struct IDxcCompiler3;
struct IDxcContainerReflection;

class FShaderCompileHelper
{
public:

	

	static FShaderCompileResult CompileShader(FShaderCompileArguments InShaderCompileArguments);

private:

	static ComPtr<IDxcLibrary> GetDxcLibrary();
	static ComPtr<IDxcUtils> GetDxcUtiles();
	static ComPtr<IDxcCompiler3> GetDxcCompilerInstance();
	static ComPtr<IDxcContainerReflection> GetDxcContainerReflectionInstance();
};

