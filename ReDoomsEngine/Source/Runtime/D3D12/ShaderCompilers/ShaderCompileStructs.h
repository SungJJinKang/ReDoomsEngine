#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "DirectXShaderCompiler/inc/d3d12shader.h"

struct FShaderPreprocessorDefine
{
	eastl::wstring Name;
	eastl::wstring Value;
};

enum EShaderCompileFlag : uint64_t
{
	None = 0,
	Allow16BitTYpes = 1 << 0
};

struct FShaderDeclaration
{
	const wchar_t* ShaderName;
	const wchar_t* ShaderTextFilePath;

	const wchar_t* ShaderEntryPoint;
	EShaderFrequency ShaderFrequency;
	uint64_t ShaderCompileFlags;
	eastl::vector<FShaderPreprocessorDefine> AdditionalPreprocessorDefineList;
};

struct FShaderCompileArguments
{
	enum class EOptimizationLevel
	{
		O0,
		O1,
		O2,
		O3,
		DisableOptimization
	};

	enum class EWarningLevel
	{
		W0,
		W1,
		W2,
		W3,
		All
	};

	// Below data is set from FD3D12Shader
	FShaderDeclaration ShaderDeclaration{};
	const char* ShaderText = nullptr;
	uint64_t ShaderTextLength = 0;

	// Below datas is set from shader compiler
	EOptimizationLevel OptimizationLevel = EOptimizationLevel::DisableOptimization;
	EWarningLevel WarningLevel = EWarningLevel::W1;
	bool bGenerateDebugInformation = false;
	bool bWarningAsError = false;
	bool bGenerateSymbols = false;

	static const wchar_t* ConvertShaderFrequencyToShaderProfile(const EShaderFrequency InShaderFrequency);
	static FShaderPreprocessorDefine ParseDefineStr(const wchar_t* const Str);
};

#define SHADER_HASH_SIZE 16


struct FShaderHash
{
	alignas(16) uint8_t Value[SHADER_HASH_SIZE];
};
bool operator<(const FShaderHash& A, const FShaderHash& B);

struct FShaderCompileResult
{
	bool bIsValid;
	eastl::vector<uint8_t> ShaderBlobData;
	ComPtr<ID3D12ShaderReflection> DxcContainerReflection; // https://learn.microsoft.com/en-us/windows/win32/api/d3d12shader/nn-d3d12shader-id3d12shaderreflection
	FShaderHash ShaderHash;
};
