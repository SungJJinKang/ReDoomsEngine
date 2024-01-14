#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"

class FD3D12RootSignature;
class FD3D12ShaderTemplate;
struct FShaderVariableTemplate;

template<typename Type>
struct FShaderVariable;

struct FShaderPreprocessorDefineAdd;

struct FBoundShaderSet
{
	FBoundShaderSet() = delete;
	FBoundShaderSet(const eastl::array<FD3D12ShaderTemplate*, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH> InShaderList);
	void CacheHash();
	void Validate();

	const eastl::array<FD3D12ShaderTemplate*, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH> ShaderList{ nullptr };
	FHash CachedHash;
};

struct FD3D12ConstantBufferReflectionData
{
	struct FD3D12VariableOfConstantBufferReflectionData
	{
		eastl::string Name;
		D3D12_SHADER_VARIABLE_DESC Desc;
	};

	eastl::string Name;
	D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
	D3D12_SHADER_BUFFER_DESC Desc;
	eastl::vector<FD3D12VariableOfConstantBufferReflectionData> VariableList; // variables declared in cbuffer
};

struct FD3D12ShaderReflectionData
{
	D3D12_SHADER_DESC ShaderDesc;

	// Input Elements
	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;

	// A InputElement info is stored at the same pos(index) in InputElementSignatureParameterList and InputElementDescList and InputElementSemanticNameList
	eastl::vector<D3D12_SIGNATURE_PARAMETER_DESC> InputElementSignatureParameterList;
	eastl::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescList;
	eastl::vector<eastl::string> InputElementSemanticNameList;

	// Output Elements
	eastl::vector<D3D12_SIGNATURE_PARAMETER_DESC> OutputElementSignatureParameterList;
	eastl::vector<eastl::string> OutputElementSemanticNameList;

	// Resource Binding
	eastl::vector<eastl::string> ResourceBindingNameList;

	// A Constant buffer info is stored at the same pos(index) in ConstantBufferResourceBindingDescList and ConstantBufferDescList
	eastl::vector<FD3D12ConstantBufferReflectionData> ConstantBufferList;

	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> TextureResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> SamplerResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> UAVResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> ByteAddressBufferResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> StructuredBufferResourceBindingDescList;

	uint32_t ShaderResourceCount;
	uint32_t ConstantBufferCount;
	uint32_t SamplerCount;
	uint32_t UnorderedAccessCount;
};

class FD3D12ShaderTemplate
{
public:

	FD3D12ShaderTemplate(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency,
		const uint64_t InShaderCompileFlags);

	void SetShaderCompileResult(FShaderCompileResult& InShaderCompileResult);
	void PopulateShaderReflectionData(ID3D12ShaderReflection* const InD3D12ShaderReflection);
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
		return ShaderBlobData;
	}
	const FD3D12ShaderReflectionData& GetD3D12ShaderReflection() const
	{
		return ShaderReflectionData;
	}
	const FHash& GetShaderHash() const
	{
		return ShaderHash;
	}

	void AddShaderVariable(FShaderVariableTemplate& InShaderVariable);
	void AddShaderPreprocessorDefine(const FShaderPreprocessorDefine& InShaderPreprocessorDefine);

private:

	FShaderDeclaration ShaderDeclaration; // ShaderCompileResult variable also contains ShaderDeclaration same with this variable. 

	eastl::vector<uint8_t> ShaderBlobData;
	FD3D12ShaderReflectionData ShaderReflectionData;
	FHash ShaderHash;
	eastl::shared_ptr<FD3D12RootSignature> RootSignature;
};

class FD3D12ShaderInstance
{
public:

private:

};

struct FShaderVariableTemplate
{

};

template<typename VariableType>
struct FShaderVariable : public FShaderVariableTemplate
{
	FShaderVariable(FD3D12ShaderTemplate& D3D12Shader, const char* const InVariableName)
		: FShaderVariableTemplate(), VariableName(InVariableName)
	{
		MEM_ZERO(Value);
		D3D12Shader.AddShaderVariable(*this);
	}
	VariableType Value;
	const char* const VariableName;
	//
	//
};

struct FShaderPreprocessorDefineAdd
{
	FShaderPreprocessorDefineAdd(FD3D12ShaderTemplate& D3D12Shader, const char* const InDefineStr);
	const char* const DefineStr;
};

// This should match with variable declared in shader
#define ADD_SHADER_VARIABLE(Type, VariableNameStr) \
	public: \
	FShaderVariable<Type> VariableNameStr{*this, #VariableNameStr};

#define ADD_PREPROCESSOR_DEFINE(DefineStr) \
	private: \
	FShaderPreprocessorDefineAdd RD_UNIQUE_NAME(ShaderPreprocessorDefine) {*this, #DefineStr};

// TODO) Need to support permutation?
// Example 1 : DEFINE_SHADER(MotionBlurVS, "MotionBlur.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "EARLY_OUT", "FAST_PATH=1");
// Example 2 : DEFINE_SHADER(MotionBlurPS, "MotionBlur.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None);
//
// Parameters)
// ShaderTextFileRelativePath : Shader file path relative to Asset/Shader
#define DEFINE_SHADER(ShaderName, ShaderTextFileRelativePath, ShaderEntryPoint, ShaderFrequency, ShaderCompileFlags, ...) \
	class FD3D12Shader##ShaderName : public FD3D12ShaderTemplate \
	{ \
	public: \
		FD3D12Shader##ShaderName(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath, \
			const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags) \
			: FD3D12ShaderTemplate(InShaderName, InShaderTextFileRelativePath, InShaderEntryPoint, InShaderFrequency, InShaderCompileFlags) \
		{ \
		} \
		__VA_ARGS__ \
	}; \
	FD3D12Shader##ShaderName D3D12Shader##ShaderName{EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFileRelativePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags};

class FD3D12ShaderManager : public EA::StdC::Singleton<FD3D12ShaderManager>
{
public:

	void Init();
	bool CompileAndAddNewShader(FD3D12ShaderTemplate& Shader, const FShaderCompileArguments& InShaderCompileArguments);
	void CompileAllPendingShader();

	/// <summary>
	/// FD3D12Shader objects created through "DEFINE_SHADER" macros is added to this list
	/// </summary>
	static eastl::vector<FD3D12ShaderTemplate*>& GetCompilePendingShaderList();
	static void AddCompilePendingShader(FD3D12ShaderTemplate& CompilePendingShader);
	inline const eastl::vector<FD3D12ShaderTemplate*>& GetShaderList() const
	{
		return ShaderList;
	}
private:

	/// <summary>
	/// FD3D12Shader objects containing compiled data is added to this list
	/// </summary>
	eastl::vector<FD3D12ShaderTemplate*> ShaderList;

};