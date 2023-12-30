#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"

class FD3D12Shader
{
public:

	FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFilePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency,
		const uint64_t InShaderCompileFlags);
	FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFilePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, 
		const uint64_t InShaderCompileFlags, const char* const InAdditionalPreprocessorDefine ...);

	void SetShaderCompileResult(const FShaderCompileResult& InShaderCompileResult);

	const FShaderDeclaration& GetShaderDeclaration() const
	{
		return ShaderDeclaration;
	}
	EShaderFrequency GetShaderFrequency() const
	{
		return ShaderDeclaration.ShaderFrequency;
	}
	const eastl::vector<uint8_t>& GetShaderBlob() const
	{
		return ShaderCompileResult.ShaderBlobData;
	}
	const ID3D12ShaderReflection* GetD3D12ShaderReflection() const
	{
		return ShaderCompileResult.DxcContainerReflection.Get();
	}
	const FShaderHash& GetShaderHash() const
	{
		return ShaderCompileResult.ShaderHash;
	}

private:

	FShaderDeclaration ShaderDeclaration; // ShaderCompileResult variable also contains ShaderDeclaration same with this variable. 
	FShaderCompileResult ShaderCompileResult;
};

// TODO) Need to support permutation?
// Example 1 : DECLARE_SHADER(MotionBlurVS, "Asset/Shader/MotionBlur.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "EARLY_OUT", "FAST_PATH=1");
// Example 2 : DECLARE_SHADER(MotionBlurPS, "Asset/Shader/MotionBlur.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None);
#define DECLARE_SHADER(ShaderName, ShaderTextFilePath, ShaderEntryPoint, ShaderFrequency, ShaderCompileFlags, AdditionalPreprocessorDefine, ... /*Definitions*/) \
	static FD3D12Shader D3D12Shader##ShaderName{EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFilePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags, AdditionalPreprocessorDefine, ##__VA_ARGS__ };

class FD3D12ShaderManager : public EA::StdC::Singleton<FD3D12ShaderManager>
{
public:

	bool CompileAndAddNewShader(FD3D12Shader& Shader, const FShaderCompileArguments& InShaderCompileArguments);

	static void AddCompilePendingShader(FD3D12Shader& CompilePendingShader);

private:

	/// <summary>
	/// FD3D12Shader objects containing compiled data is added to this list
	/// </summary>
	eastl::vector<FD3D12Shader*> ShaderList;

	/// <summary>
	/// FD3D12Shader objects created through "DECLARE_SHADER" macros is added to this list
	/// </summary>
	static eastl::vector<FD3D12Shader*> CompilePendingShaderList;
};