#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"
#include "D3D12Enums.h"
#include "D3D12Resource/D3D12ConstantBufferHelper.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12RendererStateCallbackInterface.h"

struct FD3D12CommandContext;
class FD3D12RootSignature;
class FD3D12ShaderTemplate;
class FShaderParameterTemplate;
class FD3D12ShaderInstance;
class FShaderParameterConstantBuffer;

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

struct FD3D12ConstantBufferReflectionData
{
	struct FD3D12VariableOfConstantBufferReflectionData
	{
		eastl::string Name;
		D3D12_SHADER_VARIABLE_DESC Desc;
		D3D12_SHADER_TYPE_DESC TypeDesc;
	};

	bool bIsGlobalVariable;
	eastl::string Name;
	D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
	D3D12_SHADER_BUFFER_DESC Desc;
	eastl::hash_map<eastl::string, FD3D12VariableOfConstantBufferReflectionData> VariableList; // variables declared in cbuffer or global variables
};

// Member variables never be touched after PopulateShaderReflectionData because shader parameters reference the variables by reference
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

	eastl::vector_map<eastl::string, FShaderParameterTemplate*> ShaderParameterMap;
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
	void ApplyShaderParameters(FD3D12CommandContext& InCommandContext);

protected:

	FD3D12ShaderTemplate* D3D12ShaderTemplate;
	FD3D12ShaderInstance* D3D12ShaderInstance;
	
	bool bIsShaderInstance;

	eastl::vector<FShaderParameterTemplate*> ShaderParameterList;
};

class FShaderParameterTemplate
{
public:

	FShaderParameterTemplate()
		: ShaderParameterContainerTemplate(nullptr), VariableName()
	{

	}

	FShaderParameterTemplate(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName)
		: ShaderParameterContainerTemplate(InShaderParameter), VariableName(InVariableName)
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

	virtual bool IsConstantBuffer() const
	{
		return false;
	}
	virtual bool IsSRV() const
	{
		return false;
	}
	virtual bool IsUAV() const
	{
		return false;
	}
	virtual bool IsRTV() const
	{
		return false;
	}
	virtual bool IsTemplateVariable() const = 0;
	virtual bool HasReflectionData() const = 0;

	virtual void InitD3DResource();
	void ApplyResource(FD3D12CommandContext& InCommandContext, const FD3D12RootSignature* const InRootSignature);

protected:

	FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate;
	const char* const VariableName;

private:

	virtual void SetReflectionDataFromShaderReflectionData() = 0;
};
 
class FShaderParameterResourceView : public FShaderParameterTemplate
{
public:

	FShaderParameterResourceView()
		: FShaderParameterTemplate(), ReflectionData()
	{
	}

	FShaderParameterResourceView(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName)
		: FShaderParameterTemplate(InShaderParameter, InVariableName), ReflectionData()
	{
	}

	virtual void InitD3DResource(){}
	const D3D12_SHADER_INPUT_BIND_DESC& GetReflectionData() const
	{
		return *ReflectionData;
	}
	virtual bool HasReflectionData() const
	{
		return ReflectionData;
	}

protected:

	const D3D12_SHADER_INPUT_BIND_DESC* ReflectionData;
};

class FShaderParameterShaderResourceView : public FShaderParameterResourceView
{
public:
 
	FShaderParameterShaderResourceView()
 		: FShaderParameterResourceView(), TargetSRV(nullptr)
	{
	}
 
	FShaderParameterShaderResourceView(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName)
 	: FShaderParameterResourceView(InShaderParameter, InVariableName), TargetSRV(nullptr)
	{
	}

	virtual bool IsTemplateVariable() const { return false; }

	virtual bool IsSRV() const
	{
		return true;
	}
 
	FD3D12ShaderResourceView* GetTargetSRV() const
	{
		return TargetSRV;
	}
	FShaderParameterShaderResourceView& operator=(FD3D12ShaderResourceView* const InSRV);

private:
 
	virtual void SetReflectionDataFromShaderReflectionData();
 
	FD3D12ShaderResourceView* TargetSRV;
};


class FShaderParameterConstantBuffer : public FShaderParameterTemplate
{
public:
	struct FMemberVariableContainer
	{
		const char* VariableName;
		FShaderParameterConstantBufferMemberVariableTemplate* ShaderParameterConstantBufferMemberVariableTemplate;
		uint64_t VariableSize;
		const std::type_info* VariableTypeInfo;
	};

	FShaderParameterConstantBuffer()
		: FShaderParameterTemplate(), bGlobalConstantBuffer(false), MemberVariableMap(), ReflectionData(nullptr)
	{
	}

	FShaderParameterConstantBuffer(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bInGlobalConstantBuffer)
		: FShaderParameterTemplate(InShaderParameter, InVariableName), bGlobalConstantBuffer(bInGlobalConstantBuffer), MemberVariableMap(), ReflectionData(nullptr)
	{
	}

	virtual void InitD3DResource();
	void AddMemberVariable(FShaderParameterConstantBufferMemberVariableTemplate* InShaderParameterConstantBufferMemberVariable, const uint64_t InVariableSize, 
		const char* const InVariableName, const std::type_info& InTypeId);

	virtual bool IsConstantBuffer() const
	{
		return true;
	}
	virtual uint64_t GetSize() const = 0;

	inline bool IsGlobalConstantBuffer() const
	{
		return bGlobalConstantBuffer;
	}
	const eastl::vector_map<eastl::string, FMemberVariableContainer>& GetMemberVariableMap() const
	{
		return MemberVariableMap;
	}
	virtual FD3D12ConstantBufferResource* GetConstantBufferResource() = 0;
	const FD3D12ConstantBufferReflectionData* GetConstantBufferReflectionData() const 
	{
		return ReflectionData;
	}
	const FD3D12ConstantBufferReflectionData* GetReflectionData() const
	{
		return ReflectionData;
	}
	virtual bool HasReflectionData() const
	{
		return GetReflectionData() != nullptr;
	}
	void FlushShadowData(uint8* const InShadowDataAddress);

protected:

	bool bGlobalConstantBuffer;
	eastl::vector_map<eastl::string, FMemberVariableContainer> MemberVariableMap;

private:

	virtual void SetReflectionDataFromShaderReflectionData();

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
};

class FShaderParameterConstantBufferMemberVariableTemplate
{
public:
};

template<typename VariableType>
class TShaderParameterConstantBufferMemberVariable : public FShaderParameterConstantBufferMemberVariableTemplate
{
public:

	TShaderParameterConstantBufferMemberVariable(FShaderParameterConstantBuffer* ConstantBuffer, const char* const InVariableName)
	{
		EA_ASSERT(ConstantBuffer);
		
		MEM_ZERO(Value);
		ConstantBuffer->AddMemberVariable(this, sizeof(TShaderParameterConstantBufferMemberVariable<VariableType>), InVariableName, typeid(VariableType));
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

	void ApplyShaderParameter(FD3D12CommandContext& InCommandContext);

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
		Parameter.Init();
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
	} ShaderParameter{this};	

#define SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, ...) \
	class FConstantBufferType##ConstantBufferTypeName : public FShaderParameterConstantBuffer { \
		public: \
		FConstantBufferType##ConstantBufferTypeName() \
		: FShaderParameterConstantBuffer(), SetConstructingVariable(this), ConstantBufferResource(), MemberVariables() \
		{ \
			EA_ASSERT(TemplateVariable == nullptr); \
			TemplateVariable = this; \
			ConstructingVariable = nullptr; \
		} \
		struct FMemberVariableContainer; \
		FConstantBufferType##ConstantBufferTypeName(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bIsGlobalConstantBuffer) \
		: FShaderParameterConstantBuffer(InShaderParameter, InVariableName, bIsGlobalConstantBuffer), SetConstructingVariable(this), ConstantBufferResource(), MemberVariables() \
		{ \
			if(!TemplateVariable) TemplateVariable = this; \
			if(!IsTemplateVariable()) ConstantBufferResource = eastl::make_unique<TD3D12ConstantBufferResource<FMemberVariableContainer>>(true, reinterpret_cast<uint8*>(&MemberVariables)); \
			ConstructingVariable = nullptr; \
		} \
		private: \
		inline static FConstantBufferType##ConstantBufferTypeName* ConstructingVariable = nullptr; \
		struct FSetConstructingVariable \
		{ \
			FSetConstructingVariable(FConstantBufferType##ConstantBufferTypeName* InConstructingVariable) \
			{ \
				ConstructingVariable = InConstructingVariable; \
			} \
		} SetConstructingVariable; \
		inline static FConstantBufferType##ConstantBufferTypeName* TemplateVariable = nullptr; \
		eastl::unique_ptr<TD3D12ConstantBufferResource<FMemberVariableContainer>> ConstantBufferResource; \
		public: \
		struct FMemberVariableContainer \
		{ \
			__VA_ARGS__ \
		} MemberVariables; \
		virtual uint64_t GetSize() const { return sizeof(FMemberVariableContainer); } \
		virtual bool IsTemplateVariable() const { return (TemplateVariable == this);} \
		virtual FD3D12ConstantBufferResource* GetConstantBufferResource()  { return ConstantBufferResource.get(); } \
		FMemberVariableContainer* operator->(){ return &MemberVariables; } \
	}  

#define DEFINE_SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, ...) \
	SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, __VA_ARGS__) ConstantBufferTypeName{};

#define ADD_SHADER_CONSTANT_BUFFER(ConstantBufferTypeName, VariableNameStr) \
	public: \
	FConstantBufferType##ConstantBufferTypeName VariableNameStr{this, #VariableNameStr, false};

#define ADD_SHADER_GLOBAL_CONSTANT_BUFFER(...) \
	public: \
	SHADER_CONSTANT_BUFFER_TYPE(GlobalConstantBuffer, __VA_ARGS__) GlobalConstantBuffer{this, GLOBAL_CONSTANT_BUFFER_NAME, true};

// This should match with variable declared in shader
#define ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Type, VariableNameStr) \
	static_assert(sizeof(TShaderParameterConstantBufferMemberVariable<Type>) == sizeof(FD3D12ConstantBufferMemberVariableHelper<Type>::AlignedType)); \
	TShaderParameterConstantBufferMemberVariable<Type> VariableNameStr{ConstructingVariable, #VariableNameStr};

#define ADD_SHADER_SRV_VARIABLE(VariableNameStr) \
	public: \
	FShaderParameterShaderResourceView VariableNameStr{this, #VariableNameStr};

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

class FD3D12ShaderManager : public EA::StdC::Singleton<FD3D12ShaderManager>, public ID3D12RendererStateCallbackInterface
{
public:

	void Init();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

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