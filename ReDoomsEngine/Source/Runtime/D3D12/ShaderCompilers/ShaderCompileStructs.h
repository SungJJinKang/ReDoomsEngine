#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "DirectXShaderCompiler/inc/d3d12shader.h"

struct FShaderPreprocessorDefine
{
	eastl::wstring Name;
	eastl::wstring Value;

	bool operator==(const FShaderPreprocessorDefine& Other)
	{
		return (Name == Other.Name) && (Value == Other.Value);
	}
};

enum EShaderCompileFlag : uint64_t
{
	None = 0,
	Allow16BitTYpes = 1 << 0
};

struct FShaderDeclaration
{
	const wchar_t* ShaderName;
	const wchar_t* ShaderTextFileRelativePath;

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
	eastl::u8string_view ShaderText{};
	eastl::wstring_view ShaderTextFilePath;
	uint64_t ShaderTextLength = 0;

	// Below datas is set from shader compiler
	EOptimizationLevel OptimizationLevel = EOptimizationLevel::DisableOptimization;
	EWarningLevel WarningLevel = EWarningLevel::W0;
	bool bGenerateDebugInformation = false;
	bool bWarningAsError = false;

	// embed shader symbol data in shader container
	// this data will be used for debugging, profiling
	bool bGenerateSymbols = true;

	static const wchar_t* ConvertShaderFrequencyToShaderProfile(const EShaderFrequency InShaderFrequency);
	static FShaderPreprocessorDefine ParseDefineStr(const char* const Str);
};

using FHash = uint64_t;
#define SHADER_HASH_SIZE sizeof(FHash)

inline FHash CombineHash(const FHash& A, const FHash& B)
{
	FHash NewHash = A ^ B;
	return NewHash;
}

struct FShaderCompileResult
{
	bool bIsValid;
	eastl::vector<uint8_t> ShaderBlobData;
	ComPtr<ID3D12ShaderReflection> DxcContainerReflection; // https://learn.microsoft.com/en-us/windows/win32/api/d3d12shader/nn-d3d12shader-id3d12shaderreflection
	FHash ShaderHash;
	D3D12_SHADER_DESC ShaderDesc;

};
