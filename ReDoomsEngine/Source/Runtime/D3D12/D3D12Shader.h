#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"

struct FD3D12ShaderReflectionData
{
	D3D12_SHADER_DESC ShaderDesc;

	// Input Elements
	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	eastl::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescList;
	eastl::vector<eastl::string> InputElementSemanticNameList;

	/*
	// Output Elements
	D3D12_INPUT_LAYOUT_DESC OutputLayoutDesc;
	eastl::vector<D3D12_OUTPUT_ELEMENT_DESC> OutputElementDescList;
	eastl::vector<eastl::string> OutputElementSemanticNameList;
	*/

	// A Constant buffer info is stored at the same pos(index) in ConstantBufferResourceBindingDescList and ConstantBufferDescList
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> ConstantBufferResourceBindingDescList;
	eastl::vector<D3D12_SHADER_BUFFER_DESC> ConstantBufferDescList;

	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> TextureResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> SamplerResourceBindingDescList;
};

class FD3D12Shader
{
public:

	FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFilePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency,
		const uint64_t InShaderCompileFlags);
	FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, 
		const uint64_t InShaderCompileFlags, const char* const InAdditionalPreprocessorDefine ...);

	void SetShaderCompileResult(const FShaderCompileResult& InShaderCompileResult);
	void OnFinishShaderCompile();

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

	void PopulateShaderReflectionData();

	FShaderDeclaration ShaderDeclaration; // ShaderCompileResult variable also contains ShaderDeclaration same with this variable. 
	FShaderCompileResult ShaderCompileResult;
	FD3D12ShaderReflectionData ShaderReflectionData;
};

// TODO) Need to support permutation?
// Example 1 : DECLARE_SHADER(MotionBlurVS, "MotionBlur.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "EARLY_OUT", "FAST_PATH=1");
// Example 2 : DECLARE_SHADER(MotionBlurPS, "MotionBlur.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None);
//
// Parameters)
// ShaderTextFileRelativePath : Shader file path relative to Asset/Shader
#define DECLARE_SHADER(ShaderName, ShaderTextFileRelativePath, ShaderEntryPoint, ShaderFrequency, ShaderCompileFlags, AdditionalPreprocessorDefine, ... /*Definitions*/) \
	FD3D12Shader D3D12Shader##ShaderName{EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFileRelativePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags, AdditionalPreprocessorDefine, ##__VA_ARGS__, NULL };

class FD3D12ShaderManager : public EA::StdC::Singleton<FD3D12ShaderManager>
{
public:

	void Init();
	bool CompileAndAddNewShader(FD3D12Shader& Shader, const FShaderCompileArguments& InShaderCompileArguments);
	void CompileAllPendingShader();

	/// <summary>
	/// FD3D12Shader objects created through "DECLARE_SHADER" macros is added to this list
	/// </summary>
	static eastl::vector<FD3D12Shader*>& GetCompilePendingShaderList();
	static void AddCompilePendingShader(FD3D12Shader& CompilePendingShader);

private:

	/// <summary>
	/// FD3D12Shader objects containing compiled data is added to this list
	/// </summary>
	eastl::vector<FD3D12Shader*> ShaderList;

};