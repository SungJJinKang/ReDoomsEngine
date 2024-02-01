#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"
#include "D3D12Enums.h"
#include "D3D12Resource/D3D12ConstantBufferHelper.h"
#include "D3D12Resource/D3D12Resource.h"

struct FD3D12CommandContext;
class FD3D12RootSignature;
class FD3D12ShaderTemplate;
class FShaderParameterTemplate;
class FD3D12ShaderInstance;
class FShaderConstantBuffer;

template<typename Type>
class TShaderParameter;

class FShaderParameterConstantBufferMemberVariableTemplate;

template<typename VariableType>
class TShaderParameterConstantBufferMemberVariable;

struct FShaderPreprocessorDefineAdd;

#define GLOBAL_CONSTANT_BUFFER_NAME "$Globals"

struct FBoundShaderSet
{
	FBoundShaderSet() = default;
	FBoundShaderSet(const eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency> InShaderList);
	void CacheHash();
	void Validate();

	eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency> ShaderList{ nullptr };
	FShaderHash CachedHash;

	FD3D12RootSignature* GetRootSignature() const;
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
	virtual void OnFinishShaderCompile();

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

	void AddShaderParameter(FShaderParameterTemplate* InShaderParameter, const char* const InVariableName);
	void AddShaderPreprocessorDefine(const FShaderPreprocessorDefine& InShaderPreprocessorDefine);

	void ValidateShaderParameter();

	inline bool IsFinishToCompile() const 
	{
		return bFinishToCompile;
	}

private:

	bool bFinishToCompile;

	FShaderDeclaration ShaderDeclaration; // ShaderCompileResult variable also contains ShaderDeclaration same with this variable. 

	ComPtr<IDxcBlob> ShaderBlobData;
	FD3D12ShaderReflectionData ShaderReflectionData;
	FShaderHash ShaderHash;

	eastl::vector_map<const char*, FShaderParameterTemplate*> ShaderParameterMap;
};

class FShaderParameterContainerTemplate
{
public:

	FShaderParameterContainerTemplate(FD3D12ShaderTemplate* InD3D12ShaderTemplate)
		: D3D12ShaderTemplate(InD3D12ShaderTemplate), D3D12ShaderInstance(), bIsShaderInstance(false), ShaderParameterList()
	{
	}
	FShaderParameterContainerTemplate(FD3D12ShaderTemplate* InD3D12ShaderTemplate, FD3D12ShaderInstance* InD3D12ShaderInstance)
		: D3D12ShaderTemplate(InD3D12ShaderTemplate), D3D12ShaderInstance(InD3D12ShaderInstance), bIsShaderInstance(true), ShaderParameterList()
	{
	}

	void Init();

	FD3D12ShaderTemplate* GetD3D12ShaderTemplate() const
	{
		return D3D12ShaderTemplate;
	}
	FD3D12ShaderInstance* GetD3D12ShaderInstance() const
	{
		return D3D12ShaderInstance;
	}
	inline bool IsShaderInstance() const 
	{
		return bIsShaderInstance;
	}

	void AddShaderParamter(FShaderParameterTemplate* const InShaderParameter);
	void AllocateResources();
	void ApplyShaderParameters(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature);

protected:

	FD3D12ShaderTemplate* D3D12ShaderTemplate;
	FD3D12ShaderInstance* D3D12ShaderInstance;
	
	bool bIsShaderInstance;

	eastl::vector<FShaderConstantBuffer*> ConstantBufferList;

	eastl::vector<FShaderParameterTemplate*> ShaderParameterList;
};

class FShaderParameterTemplate
{
public:
	FShaderParameterTemplate(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName)
		: OwnerShaderParameterContainerTemplate(InShaderParameter), VariableName(InVariableName)
	{
		InShaderParameter->AddShaderParamter(this);

		if (InShaderParameter->GetD3D12ShaderInstance())
		{

		}
		else
		{
			InShaderParameter->GetD3D12ShaderTemplate()->AddShaderParameter(this, VariableName);
		}
	}

	virtual void Init();

	const char* const GetVariableName() const
	{
		return VariableName;
	}

	virtual uint64_t GetSize() const = 0;
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
	virtual bool IsTemplateVariable() const = 0;

	void AllocateResource();
	void ApplyResource(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature);
	virtual FD3D12Resource* GetD3D12Resource() = 0;

protected:

	FShaderParameterContainerTemplate* const OwnerShaderParameterContainerTemplate;
	const char* const VariableName;

private:

	virtual void SetReflectionDataFromShader() = 0;
};

class FShaderConstantBuffer : public FShaderParameterTemplate
{
public:
	struct FMemberVariableContainer
	{
		const char* VariableName;
		FShaderParameterConstantBufferMemberVariableTemplate* ShaderParameterConstantBufferMemberVariableTemplate;
		size_t VariableSize;
	};

	FShaderConstantBuffer(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bInGlobalConstantBuffer)
		: FShaderParameterTemplate(InShaderParameter, InVariableName), bGlobalConstantBuffer(bInGlobalConstantBuffer), MemberVariableMap()
	{
	}

	template<typename VariableType>
	void AddMemberVariable(TShaderParameterConstantBufferMemberVariable<VariableType>* InShaderParameterConstantBufferMemberVariable, const char* const InVariableName)
	{
		EA_ASSERT(MemberVariableMap.find(InVariableName) == MemberVariableMap.end());

		FMemberVariableContainer MemberVariableContainer;
		MemberVariableContainer.VariableName = InVariableName;
		MemberVariableContainer.ShaderParameterConstantBufferMemberVariableTemplate = InShaderParameterConstantBufferMemberVariable;
		MemberVariableContainer.VariableSize = sizeof(TShaderParameterConstantBufferMemberVariable<VariableType>);

		MemberVariableMap.emplace(InVariableName, MemberVariableContainer);
	}

	virtual bool IsConstantBuffer() const
	{
		return true;
	}

	virtual uint64_t GetSize() const = 0;
	virtual uint8_t* GetData() = 0;


	inline bool IsGlobalConstantBuffer() const
	{
		return bGlobalConstantBuffer;
	}
	const eastl::vector_map<const char*, FMemberVariableContainer>& GetMemberVariableMap() const
	{
		return MemberVariableMap;
	}
	virtual FD3D12ConstantBufferResource* GetConstantBufferResource() = 0;
	const FD3D12ConstantBufferReflectionData* GetConstantBufferReflectionData() const 
	{
		return ReflectionData;
	}

protected:

	bool bGlobalConstantBuffer;
	eastl::vector_map<const char*, FMemberVariableContainer> MemberVariableMap;

private:

	virtual void SetReflectionDataFromShader();

	const FD3D12ConstantBufferReflectionData* ReflectionData;
};

template<typename VariableType>
class TShaderParameter : public FShaderParameterTemplate
{
	TShaderParameter(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName)
		: FShaderParameterTemplate(InShaderParameter, InVariableName)
	{
		MEM_ZERO(Value);
	}

	VariableType Value;

	std::type_info GetTypeId() const
	{
		return typeid(VariableType);
	}

	virtual uint64_t GetSize() const
	{
		return GetShaderVariableSize<VariableType>();
	}

	virtual bool IsTexture() const
	{
		// @todo implement
		return false;
	}
	virtual bool IsBuffer() const
	{
		// @todo implement
		return false;
	}
	virtual bool IsSampler() const
	{
		// @todo implement
		return false;
	}
};

class FShaderParameterConstantBufferMemberVariableTemplate
{

};

template<typename VariableType>
class TShaderParameterConstantBufferMemberVariable : public FShaderParameterConstantBufferMemberVariableTemplate
{
public:
	TShaderParameterConstantBufferMemberVariable(FShaderConstantBuffer& ConstantBuffer, const char* const InVariableName)
	{
		MEM_ZERO(Value);
		ConstantBuffer.AddMemberVariable(this, InVariableName);
	}

	using VariableAlignedType = typename FD3D12ConstantBufferMemberVariableHelper<VariableType>::AlignedType;
	VariableAlignedType Value;

	operator VariableType& ()
	{
		return reinterpret_cast<VariableType&>(*Value.mCharData);
	}
	TShaderParameterConstantBufferMemberVariable& operator=(const VariableType& InValue)
	{
		static_cast<VariableType&>(*this) = InValue;
		return *this;
	}
};

struct FShaderPreprocessorDefineAdd
{
	FShaderPreprocessorDefineAdd(FD3D12ShaderTemplate& D3D12Shader, const char* const InDefineStr);
	const char* const DefineStr;
};

class FD3D12ShaderInstance
{
public:

	FD3D12ShaderInstance(FD3D12ShaderTemplate* InShaderTemplate, FShaderParameterContainerTemplate* const InShaderParameterContainerTemplate)
		: ShaderTemplate(InShaderTemplate), ShaderParameterContainerTemplate(InShaderParameterContainerTemplate)
	{

	}

	inline FD3D12ShaderTemplate* GetShaderTemplate() const
	{
		return ShaderTemplate;
	}

	inline FShaderParameterContainerTemplate* GetShaderParameterContainer() const
	{
		return ShaderParameterContainerTemplate;
	}

	void ApplyShaderParameter(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature);

protected:

	FD3D12ShaderTemplate* const ShaderTemplate;
	FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate;

};

template <typename ShaderType>
class TD3D12ShaderInstance : public FD3D12ShaderInstance
{
public:

	TD3D12ShaderInstance(FD3D12ShaderTemplate* InShaderTemplate)
		: FD3D12ShaderInstance(InShaderTemplate, &Parameter), Parameter(InShaderTemplate, this)
	{

	}

	void Init()
	{
		Parameter.AllocateResources();
	}

	typename ShaderType::FShaderParameterContainer Parameter;

private:

};

#define DEFINE_SHADER_PARAMTERS(...) \
	public: \
	class FShaderParameterContainer : public FShaderParameterContainerTemplate \
	{ \
		public: \
		FShaderParameterContainer(FD3D12ShaderTemplate* InD3D12ShaderTemplate) \
			: FShaderParameterContainerTemplate(InD3D12ShaderTemplate) {} \
		FShaderParameterContainer(FD3D12ShaderTemplate* InD3D12ShaderTemplate, FD3D12ShaderInstance* InD3D12ShaderInstance) \
			: FShaderParameterContainerTemplate(InD3D12ShaderTemplate, InD3D12ShaderInstance) {} \
	__VA_ARGS__ \
	} ShaderParameter{this}; \

#define DEFINE_SHADER_GLOBAL_CONSTANT_BUFFER(...) \
	public: \
	struct FGlobalConstantBufferType : public FShaderConstantBuffer { \
		FGlobalConstantBufferType(FShaderParameterContainer* InShaderParameter) \
		: FShaderConstantBuffer(InShaderParameter, GLOBAL_CONSTANT_BUFFER_NAME, true), ConstantBufferResource(true) \
		{ \
			if(!TemplateVariable) TemplateVariable = this; \
		} \
		inline static FGlobalConstantBufferType* TemplateVariable = nullptr; \
		struct FData \
		{ \
			__VA_ARGS__ \
		};\
		virtual bool IsTemplateVariable() const { return (TemplateVariable == this);} \
		private: \
		TD3D12ConstantBufferResource<FData> ConstantBufferResource; \
		virtual uint8_t* GetData() { return reinterpret_cast<uint8_t*>(&(ConstantBufferResource.Data())); } \
		virtual uint64_t GetSize() const { return sizeof(FData); } \
		public: \
		FData& Data() { return ConstantBufferResource.Data(); } \
		virtual FD3D12Resource* GetD3D12Resource() { return &ConstantBufferResource; } \
		virtual FD3D12ConstantBufferResource* GetConstantBufferResource()  { return &ConstantBufferResource; } \
	} GlobalConstantBuffer{this}; 

// This should match with variable declared in shader
#define ADD_SHADER_GLOBAL_CONSTANT_BUFFER_MEMBER_VARIABLE(Type, VariableNameStr) \
	public: \
	static_assert(sizeof(TShaderParameterConstantBufferMemberVariable<Type>) == sizeof(FD3D12ConstantBufferMemberVariableHelper<Type>::AlignedType));\
	TShaderParameterConstantBufferMemberVariable<Type> VariableNameStr{*TemplateVariable, #VariableNameStr};

#define ADD_SHADER_SRV_VARIABLE
#define ADD_SHADER_UAV_VARIABLE

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
	class F##ShaderName : public FD3D12ShaderTemplate \
	{ \
	public: \
		F##ShaderName() = delete; \
		F##ShaderName(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath, \
			const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags) \
			: FD3D12ShaderTemplate(InShaderName, InShaderTextFileRelativePath, InShaderEntryPoint, InShaderFrequency, InShaderCompileFlags) \
		{ \
			EA_ASSERT(TemplateVariable == nullptr); \
			TemplateVariable = this; \
		} \
		__VA_ARGS__ \
		virtual void OnFinishShaderCompile() { FD3D12ShaderTemplate::OnFinishShaderCompile(); ShaderParameter.Init(); } \
		static eastl::unique_ptr<TD3D12ShaderInstance<F##ShaderName>> MakeShaderInstance()  \
		{ \
			EA_ASSERT(TemplateVariable->IsFinishToCompile()); \
			eastl::unique_ptr<TD3D12ShaderInstance<F##ShaderName>> ShaderInstance = eastl::make_unique<TD3D12ShaderInstance<F##ShaderName>>(TemplateVariable); \
			ShaderInstance->Init(); \
			return ShaderInstance;	\
		} \
		private: \
		inline static F##ShaderName* TemplateVariable = nullptr; \
	}; \
	static F##ShaderName ShaderName{ EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFileRelativePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags };

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