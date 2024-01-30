#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"
#include "D3D12Enums.h"

class FD3D12RootSignature;
class FD3D12ShaderTemplate;
struct FShaderParameterTemplate;

template<typename Type>
struct FShaderParameter;

struct FShaderPreprocessorDefineAdd;

struct FBoundShaderSet
{
	FBoundShaderSet() = default;
	FBoundShaderSet(const eastl::array<FD3D12ShaderTemplate*, D3D12_SHADER_VISIBILITY_NUM> InShaderList);
	void CacheHash();
	void Validate();

	eastl::array<FD3D12ShaderTemplate*, D3D12_SHADER_VISIBILITY_NUM> ShaderList{ nullptr };
	FShaderHash CachedHash;
};

inline bool operator==(const FBoundShaderSet& lhs, const FBoundShaderSet& rhs)
{
	return lhs.CachedHash == rhs.CachedHash;
}
inline bool operator!=(const FBoundShaderSet& lhs, const FBoundShaderSet& rhs)
{
	return lhs.CachedHash != rhs.CachedHash;
}

//
template<typename Type>
uint32_t GetShaderVariableSize()
{
	return sizeof(Type);
}

struct FD3D12ConstantBufferReflectionData
{
	struct FD3D12VariableOfConstantBufferReflectionData
	{
		eastl::string Name;
		D3D12_SHADER_VARIABLE_DESC Desc;
	};

	bool bIsGlobalVariable;
	eastl::string Name;
	D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
	D3D12_SHADER_BUFFER_DESC Desc;
	eastl::vector<FD3D12VariableOfConstantBufferReflectionData> VariableList; // variables declared in cbuffer
};

struct FD3D12ShaderReflectionData
{
	bool bPopulated = false;

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
	FD3D12ConstantBufferReflectionData GlobalConstantBuffer;
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
	ComPtr<IDxcBlob>& GetShaderBlob()
	{
		return ShaderBlobData;
	}
	const FD3D12ShaderReflectionData& GetD3D12ShaderReflection() const
	{
		return ShaderReflectionData;
	}
	const FShaderHash& GetShaderHash() const
	{
		return ShaderHash;
	}

	void AddShaderParameter(FShaderParameterTemplate* InShaderParameter);
	void AddShaderPreprocessorDefine(const FShaderPreprocessorDefine& InShaderPreprocessorDefine);

	void ValidateShaderParameter();

private:

	FShaderDeclaration ShaderDeclaration; // ShaderCompileResult variable also contains ShaderDeclaration same with this variable. 

	ComPtr<IDxcBlob> ShaderBlobData;
	FD3D12ShaderReflectionData ShaderReflectionData;
	FShaderHash ShaderHash;

	eastl::vector<FShaderParameterTemplate*> ShaderParameterList;
};

class FD3D12ShaderInstance
{
public:

private:

};

struct FShaderParameterTemplate
{
	FShaderParameterTemplate(const char* const InVariableName)
		: VariableName(InVariableName)
	{

	}
	const char* const VariableName;

	virtual uint32_t GetSize() = 0;
	virtual bool IsConstantBuffer() const
	{
		return false;
	}
	virtual bool IsTexture() const
	{
		return false;
	}
	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsSampler() const
	{
		return false;
	}
};

struct FShaderConstantBufferTemplate : public FShaderParameterTemplate
{
	FShaderConstantBufferTemplate(FD3D12ShaderTemplate& D3D12Shader, const char* const InVariableName)
		: FShaderParameterTemplate(InVariableName), MemberVariableList()
	{
		D3D12Shader.AddShaderParameter(this);
	}

	void AddMemberVariable(FShaderParameterTemplate* InShaderParameter);

	virtual bool IsConstantBuffer() const
	{
		return true;
	}

	virtual uint32_t GetSize()
	{
		EA_ASSERT(false); // todo
		return 0;
	}

	eastl::vector<FShaderParameterTemplate*> MemberVariableList;
};

template<typename VariableType>
struct FShaderParameter : public FShaderParameterTemplate
{
	FShaderParameter(FShaderConstantBufferTemplate& ConstantBuffer, const char* const InVariableName)
		: FShaderParameterTemplate(InVariableName)
	{
		MEM_ZERO(Value);
		ConstantBuffer.AddMemberVariable(this);
	}

	FShaderParameter(FD3D12ShaderTemplate& D3D12Shader, const char* const InVariableName)
		: FShaderParameterTemplate(InVariableName)
	{
		MEM_ZERO(Value);
		D3D12Shader.AddShaderParameter(this);
	}
	VariableType Value;

	std::type_info GetTypeId() const
	{
		return typeid(VariableType);
	}

	virtual uint32_t GetSize()
	{
		return GetShaderVariableSize<VariableType>();

	}

	virtual bool IsTexture() const
	{
		return false;
	}
	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsSampler() const
	{
		return false;
	}
};

struct FShaderPreprocessorDefineAdd
{
	FShaderPreprocessorDefineAdd(FD3D12ShaderTemplate& D3D12Shader, const char* const InDefineStr);
	const char* const DefineStr;
};

#define ADD_CONSTANT_BUFFER(VariableNameStr, ...) \
	public : \
	struct FShaderConstantBuffer##VariableNameStr : public FShaderConstantBufferTemplate \
	{ \
		FShaderConstantBuffer##VariableNameStr(FD3D12ShaderTemplate& D3D12Shader, const char* const InVariableName) \
			: FShaderConstantBufferTemplate(D3D12Shader, InVariableName) \
		{ \
			D3D12Shader.AddShaderParameter(this); \
		} \
		__VA_ARGS__ \
	}; \
	public: \
	FShaderConstantBuffer##VariableNameStr VariableNameStr{ *this, #VariableNameStr };

// This should match with variable declared in shader
#define ADD_SHADER_VARIABLE(Type, VariableNameStr) \
	public: \
	FShaderParameter<Type> VariableNameStr{*this, #VariableNameStr};

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
	FD3D12Shader##ShaderName ShaderName{EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFileRelativePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags}; \
	class FD3D12Shader##ShaderName##Instance : public FD3D12Shader##ShaderName {};

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