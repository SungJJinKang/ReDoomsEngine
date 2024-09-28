#pragma once
#include "CommonInclude.h"
#include "ShaderCompilers/ShaderCompileStructs.h"
#include "D3D12Enums.h"
#include "D3D12Resource/D3D12ConstantBufferHelper.h"
#include "D3D12Resource/D3D12Resource.h"
#include "Common/RendererStateCallbackInterface.h"
#include "SceneData/GPUScene.h"
#include "D3D12VertexFactory.h"

struct FD3D12CommandContext;
class FD3D12RootSignature;
class FD3D12Material;
class FD3D12Shader;
class FShaderParameterTemplate;
class FD3D12Material;
class FShaderParameterConstantBuffer;

template<typename Type>
class TShaderParameter;

class FShaderParameterConstantBufferMemberVariableTemplate;

template<typename VariableType>
class TShaderParameterConstantBufferMemberVariable;

struct FShaderPreprocessorDefineAdd;

#define GLOBAL_CONSTANT_BUFFER_NAME "$Globals"

class FBoundShaderSet
{
public:
	FBoundShaderSet() = default;
	FBoundShaderSet(const eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency>& InShaderList);
	
	void Set(const eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency>& InShaderList);
	void CacheHash() const;
	void Validate();
	FD3D12RootSignature* GetRootSignature() const;
	inline const eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency>& GetMaterialList() const
	{
		return MaterialList;
	}
	inline eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency>& GetMaterialList()
	{
		return MaterialList;
	}
	inline FShaderHash GetCachedHash() const
	{
		if (!IsValidHash())
		{
			CacheHash();
		}
		return CachedHash;
	}
	inline uint64 GetCachedHash64() const
	{
		if (!IsValidHash())
		{
			CacheHash();
		}
		return CachedHash64;
	}

	inline bool IsValidHash() const
	{
		return CachedHash.IsValid() && (CachedHash64 != 0);
	}
	bool IsValid() const;

private:

	eastl::array<FD3D12Material*, EShaderFrequency::NumShaderFrequency> MaterialList{ nullptr };
	mutable FShaderHash CachedHash{};
	mutable uint64 CachedHash64 = 0;
};

inline bool operator==(const FBoundShaderSet& lhs, const FBoundShaderSet& rhs)
{
	bool bEqual = true;
	for(int32 Index = 0; Index < EShaderFrequency::NumShaderFrequency; Index++)
	{
		if (lhs.GetMaterialList()[Index] != rhs.GetMaterialList()[Index])
		{
			bEqual = false;
			break;
		}
	}
	return bEqual;
}
inline bool operator!=(const FBoundShaderSet& lhs, const FBoundShaderSet& rhs)
{
	return !(lhs == rhs);
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
	eastl::string_view Name;
	D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
	D3D12_SHADER_BUFFER_DESC Desc;
	eastl::hash_map<eastl::string, FD3D12VariableOfConstantBufferReflectionData> VariableList; // variables declared in cbuffer or global variables
};

struct FSamplerResourceBindingDesc
{
	EStaticSamplerType SamplerType;
	D3D12_SHADER_INPUT_BIND_DESC Desc;
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
	eastl::vector<FSamplerResourceBindingDesc> SamplerResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> UAVResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> ByteAddressBufferResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> StructuredBufferResourceBindingDescList;

	uint32_t ShaderResourceCount;
	uint32_t ConstantBufferCount;
	uint32_t SamplerCount;
	uint32_t UnorderedAccessCount;
};

class FD3D12Shader
{
public:

	FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath,
		const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency,
		const uint64_t InShaderCompileFlags);

	FD3D12Shader(const FD3D12Shader&) = delete;
	FD3D12Shader(FD3D12Shader&&) = delete;
	FD3D12Shader& operator=(const FD3D12Shader&) = delete;
	FD3D12Shader& operator=(FD3D12Shader&&) = delete;

	void SetShaderCompileResult(FShaderCompileResult& InShaderCompileResult);
	void PopulateShaderReflectionData(ID3D12ShaderReflection* const InD3D12ShaderReflection);
	virtual void OnFinishShaderCompile();
	virtual void ResetUsedMaterialCountForCurrentFrame() = 0;

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

	FShaderParameterContainerTemplate(FD3D12Shader* InD3D12Shader)
		: D3D12Shader(InD3D12Shader), D3D12Material(), bIsMaterial(false), 
		ShaderParameterList(), PrimitiveSceneDataSRVIndexInShaderParameterList(-1)
	{
	}
	FShaderParameterContainerTemplate(FD3D12Shader* InD3D12Shader, FD3D12Material* InD3D12Material)
		: D3D12Shader(InD3D12Shader), D3D12Material(InD3D12Material), bIsMaterial(true), 
		ShaderParameterList(), PrimitiveSceneDataSRVIndexInShaderParameterList(-1)
	{
	}
	FShaderParameterContainerTemplate(const FShaderParameterContainerTemplate&) = delete;
	FShaderParameterContainerTemplate(FShaderParameterContainerTemplate&&) = delete;
	FShaderParameterContainerTemplate& operator=(const FShaderParameterContainerTemplate&) = delete;
	FShaderParameterContainerTemplate& operator=(FShaderParameterContainerTemplate&&) = delete;

	void Init();

	FD3D12Shader* GetD3D12Shader() const
	{
		return D3D12Shader;
	}
	FD3D12Material* GetD3D12Material() const
	{
		return D3D12Material;
	}
	inline bool IsMaterial() const 
	{
		return bIsMaterial;
	}
	eastl::vector<FShaderParameterTemplate*>& GetShaderParameterList()
	{
		return ShaderParameterList;
	}
	inline int32_t GetPrimitiveSceneDataSRVIndex() const
	{
		return PrimitiveSceneDataSRVIndexInShaderParameterList;
	}

	void AddShaderParamter(FShaderParameterTemplate* const InShaderParameter);
	void ApplyShaderParameters(FD3D12CommandContext& InCommandContext);
	void CopyFrom(const FShaderParameterContainerTemplate& InTemplate);
	FShaderParameterTemplate* FindShaderParameterTemplate(const char* const InVariableName);

protected:

	FD3D12Shader* D3D12Shader;
	FD3D12Material* D3D12Material;
	
	bool bIsMaterial;

	eastl::vector<FShaderParameterTemplate*> ShaderParameterList;

private:

	// this is fast path for RenderScene
	int32_t PrimitiveSceneDataSRVIndexInShaderParameterList;
};

class FShaderParameterTemplate
{
public:

	FShaderParameterTemplate(const char* const InVariableName)
		: ShaderParameterContainerTemplate(nullptr), VariableName(InVariableName)
	{

	}

	FShaderParameterTemplate(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bInAllowCull)
		: ShaderParameterContainerTemplate(InShaderParameter), VariableName(InVariableName), bAllowCull(bInAllowCull)
	{
		InShaderParameter->AddShaderParamter(this);

		if (InShaderParameter->GetD3D12Material())
		{

		}
		else
		{
			InShaderParameter->GetD3D12Shader()->AddShaderParameter(this, VariableName);
		}
	}
	FShaderParameterTemplate(const FShaderParameterTemplate&) = delete;
	FShaderParameterTemplate(FShaderParameterTemplate&&) = delete;
	FShaderParameterTemplate& operator=(const FShaderParameterTemplate&) = delete;
	FShaderParameterTemplate& operator=(FShaderParameterTemplate&&) = delete;

	// This function will be used for duplicating shader instance..
	// 
	// Do vs Don't in this function
	// Do : Copy shadow data of constant buffer
	// Don't : recreate d3d resource or set reflection data again because it's all done when the shader instance is created
	virtual void CopyFrom(const FShaderParameterTemplate& InTemplate);

	inline FShaderParameterContainerTemplate* GetShaderParameterContainerTemplate() const
	{
		return ShaderParameterContainerTemplate;
	}

	// Copy constructor, assignment operator doesn't require calling this function
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
	virtual bool IsCulled() const = 0;
	inline bool IsAllowCull() const
	{
		return bAllowCull;
	}

	void ApplyResource(FD3D12CommandContext& InCommandContext, const FD3D12RootSignature* const InRootSignature);

protected:

	bool bAllowCull;
	FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate;
	const char* const VariableName;

private:

	virtual void SetReflectionDataFromShaderReflectionData() = 0;
};

class FShaderParameterResourceView : public FShaderParameterTemplate
{
public:

	FShaderParameterResourceView(const char* const InVariableName)
		: FShaderParameterTemplate(InVariableName), ReflectionData()
	{
	}

	FShaderParameterResourceView(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const EShaderParameterResourceType InShaderParameterResourceType, const bool bInAllowCull)
		: FShaderParameterTemplate(InShaderParameter, InVariableName, bInAllowCull), ReflectionData(), ShaderParameterResourceType(InShaderParameterResourceType)
	{
	}
	virtual void CopyFrom(const FShaderParameterTemplate& InTemplate);

	const D3D12_SHADER_INPUT_BIND_DESC& GetReflectionData() const
	{
		return *ReflectionData;
	}
	virtual bool IsCulled() const
	{
		return ReflectionData == nullptr;
	}

	inline EShaderParameterResourceType GetShaderParameterResourceType() const
	{
		return ShaderParameterResourceType;
	}

protected:

	const D3D12_SHADER_INPUT_BIND_DESC* ReflectionData;

private:

	EShaderParameterResourceType ShaderParameterResourceType;
};

class FShaderParameterShaderResourceView : public FShaderParameterResourceView
{
public:
 
	FShaderParameterShaderResourceView(const char* const InVariableName)
 		: FShaderParameterResourceView(InVariableName), TargetSRV(nullptr)
	{
	}
 
	FShaderParameterShaderResourceView(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const EShaderParameterResourceType InShaderParameterResourceType, const bool bInAllowCull)
 	: FShaderParameterResourceView(InShaderParameter, InVariableName, InShaderParameterResourceType, bInAllowCull), TargetSRV(nullptr)
	{
	}
	virtual void CopyFrom(const FShaderParameterTemplate& InTemplate);

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

	FShaderParameterConstantBuffer(const char* const InVariableName);
	FShaderParameterConstantBuffer(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bInGlobalConstantBuffer, const bool bInIsDynamic, const bool bInAllowCull);

	virtual FShaderParameterConstantBuffer* GetTemplateShaderParameterConstantBuffer() = 0;
	virtual const FShaderParameterConstantBuffer* GetTemplateShaderParameterConstantBuffer() const = 0;

	virtual void Init();
	void AddMemberVariable(FShaderParameterConstantBufferMemberVariableTemplate* InShaderParameterConstantBufferMemberVariable, const uint64_t InVariableSize, 
		const char* const InVariableName, const std::type_info& InTypeId);
	virtual void CopyFrom(const FShaderParameterTemplate& InTemplate);

	virtual bool IsConstantBuffer() const
	{
		return true;
	}

	inline bool IsDynamicConstantBuffer() const
	{
		return bIsDynamic;
	}
	inline bool IsGlobalConstantBuffer() const
	{
		return bGlobalConstantBuffer;
	}
	const eastl::vector<FMemberVariableContainer>& GetMemberVariableMap() const
	{
		return MemberVariableMap;
	}
	const FD3D12ConstantBufferReflectionData* GetConstantBufferReflectionData() const 
	{
		return ReflectionData;
	}
	virtual bool IsCulled() const
	{
		return GetConstantBufferReflectionData() == nullptr;
	}
	uint8_t* GetShadowData();

	FD3D12ConstantBufferResource* GetConstantBufferResource();
	void FlushShadowData();
	void FlushShadowDataIfDirty();
	void MakeDirty();

	bool SetReflectionDataFromShaderReflectionData(const FD3D12ShaderReflectionData& InShaderReflection);

protected:

	bool bGlobalConstantBuffer;
	eastl::vector<FMemberVariableContainer> MemberVariableMap;

private:

	virtual void SetReflectionDataFromShaderReflectionData();

	const FD3D12ConstantBufferReflectionData* ReflectionData;

	eastl::unique_ptr<FD3D12ConstantBufferResource> ConstantBufferResource;
	eastl::vector<uint8_t> ShadowData;

	bool bIsDynamic;
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
	FShaderParameterConstantBufferMemberVariableTemplate(FShaderParameterConstantBuffer* InConstantBuffer, const char* const InVariableName, const bool bInAllowCull)
		: ConstantBuffer(InConstantBuffer), bSetOffset(false), bAllowCull(bInAllowCull), VariableName(InVariableName)
	{
		EA_ASSERT(ConstantBuffer);
	}
	FShaderParameterConstantBufferMemberVariableTemplate(const FShaderParameterConstantBufferMemberVariableTemplate&) = delete;
	FShaderParameterConstantBufferMemberVariableTemplate(FShaderParameterConstantBufferMemberVariableTemplate&&) = delete;
	FShaderParameterConstantBufferMemberVariableTemplate& operator=(const FShaderParameterConstantBufferMemberVariableTemplate&) = delete;
	FShaderParameterConstantBufferMemberVariableTemplate& operator=(FShaderParameterConstantBufferMemberVariableTemplate&&) = delete;

	void SetOffset(const uint32_t InOffset)
	{
		Offset = InOffset;
		bSetOffset = true;
	}

	uint8_t* GetShadowDataAddress() const
	{
		EA_ASSERT(bSetOffset);
		return ConstantBuffer->GetShadowData() + Offset;
	}

	inline bool IsAllowCull() const
	{
		return bAllowCull;
	}

	inline const char* const GetVariableName() const
	{
		return VariableName;
	}

protected:

	FShaderParameterConstantBuffer* ConstantBuffer;
	uint32_t Offset;

private:

	bool bSetOffset;
	bool bAllowCull;
	const char* const VariableName;
};

template<typename VariableType>
class TShaderParameterConstantBufferMemberVariable : public FShaderParameterConstantBufferMemberVariableTemplate
{
public:

	TShaderParameterConstantBufferMemberVariable(FShaderParameterConstantBuffer* InConstantBuffer, const char* const InVariableName, const bool bInAllowCull)
		: FShaderParameterConstantBufferMemberVariableTemplate(InConstantBuffer, InVariableName, bInAllowCull)
	{
		ConstantBuffer->AddMemberVariable(this, sizeof(TShaderParameterConstantBufferMemberVariable<VariableType>), InVariableName, typeid(VariableType));
	}

	TShaderParameterConstantBufferMemberVariable& operator=(const VariableType& InValue)
	{
		if (
			!(ConstantBuffer->IsCulled()) || 
			(!(ConstantBuffer->IsDynamicConstantBuffer()))
		)
		{
			if (EA::StdC::Memcmp(GetShadowDataAddress(), &InValue, sizeof(VariableType)) != 0)
			{
				EA::StdC::Memcpy(GetShadowDataAddress(), &InValue, sizeof(VariableType));
				ConstantBuffer->MakeDirty();
			}
		}
		else
		{
// 			RD_LOG(ELogVerbosity::Warning,
// 				EA_WCHAR("Fail to set value to member variable of constant buffer because the variable is culled(ShaderName : %s, ConstantBuffer : %s, VariableName : %s)"),
// 				ConstantBuffer->GetShaderParameterContainerTemplate()->GetD3D12Shader()->GetShaderDeclaration().ShaderName,
// 				ANSI_TO_WCHAR(ConstantBuffer->GetVariableName()),
// 				ANSI_TO_WCHAR(GetVariableName())
// 			);
		}
		return *this;
	}

	operator VariableType() const
	{
		return *reinterpret_cast<VariableType*>(GetShadowDataAddress());
	}
};

struct FShaderPreprocessorDefineAdd
{
	FShaderPreprocessorDefineAdd(FD3D12Shader& D3D12Shader, const char* const InDefineStr);
	const char* const DefineStr;
};

class FD3D12Material
{
public:

	FD3D12Material(FD3D12Shader* InShader, FShaderParameterContainerTemplate* const InShaderParameterContainerTemplate)
		: Shader(InShader), ShaderParameterContainerTemplate(InShaderParameterContainerTemplate)
	{

	}

	inline FD3D12Shader* GetShader() const
	{
		return Shader;
	}

	inline FShaderParameterContainerTemplate* GetShaderParameterContainer()
	{
		return ShaderParameterContainerTemplate;
	}
	inline const FShaderParameterContainerTemplate* GetShaderParameterContainer() const
	{
		return ShaderParameterContainerTemplate;
	}

	void ApplyShaderParameter(FD3D12CommandContext& InCommandContext);
	void ResetForReuse();
	bool bIsTemplateInstance;

protected:

	FD3D12Shader* const Shader;
	FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate;
};

template <typename ShaderType>
class TD3D12Material : public FD3D12Material
{
public:

	TD3D12Material(FD3D12Shader* InShader)
		: FD3D12Material(InShader, &Parameter), Parameter(InShader, this)
	{

	}

	void Init()
	{
		Parameter.Init();
	}

	typename ShaderType::FShaderParameterContainer Parameter;

protected:

private:

};

#define ADD_DEFAULT_SHADER_PARAMETER \
		ADD_SHADER_CONSTANT_BUFFER(ViewConstantBuffer, ViewConstantBuffer)

#define DEFINE_SHADER_PARAMTERS(...) \
	public: \
	class FShaderParameterContainer : public FShaderParameterContainerTemplate \
	{ \
		public: \
		FShaderParameterContainer(FD3D12Shader* InD3D12Shader) \
			: FShaderParameterContainerTemplate(InD3D12Shader) {} \
		FShaderParameterContainer(FD3D12Shader* InD3D12Shader, FD3D12Material* InD3D12Material) \
			: FShaderParameterContainerTemplate(InD3D12Shader, InD3D12Material) {} \
		__VA_ARGS__ \
		ADD_DEFAULT_SHADER_PARAMETER \
	} ShaderParameter{this};	

#define SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, bInIsDynamic, bInAllowCull, ...) \
	class FConstantBufferType##ConstantBufferTypeName : public FShaderParameterConstantBuffer { \
		public: \
		FConstantBufferType##ConstantBufferTypeName(const char* const InVariableName) \
		: FShaderParameterConstantBuffer(InVariableName), SetConstructingVariable(this) \
		{ \
			EA_ASSERT(TemplateVariable == nullptr); \
			TemplateVariable = this; \
			ConstructingVariable = nullptr; \
		} \
		struct FMemberVariableContainer; \
		FConstantBufferType##ConstantBufferTypeName(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bIsGlobalConstantBuffer) \
		: FShaderParameterConstantBuffer(InShaderParameter, InVariableName, bIsGlobalConstantBuffer, bInIsDynamic, bInAllowCull), SetConstructingVariable(this), MemberVariables() \
		{ \
			if(!TemplateVariable) TemplateVariable = this; \
			ConstructingVariable = nullptr; \
		} \
		virtual FShaderParameterConstantBuffer* GetTemplateShaderParameterConstantBuffer() { return TemplateVariable; } \
		virtual const FShaderParameterConstantBuffer* GetTemplateShaderParameterConstantBuffer() const { return TemplateVariable; } \
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
		public: \
		struct FMemberVariableContainer \
		{ \
			__VA_ARGS__ \
		} MemberVariables; \
		virtual bool IsTemplateVariable() const { return (TemplateVariable == this);} \
		FMemberVariableContainer* operator->(){ return &MemberVariables; } \
	}  

#define DEFINE_SHADER_CONSTANT_BUFFER_TYPE_INTERNAL(ConstantBufferTypeName, bInIsDynamic, bInAllowCull, ...) \
	inline SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, bInIsDynamic, bInAllowCull, __VA_ARGS__) ConstantBufferTypeName{#ConstantBufferTypeName};

#define DEFINE_SHADER_CONSTANT_BUFFER_TYPE(ConstantBufferTypeName, bInIsDynamic, ...) DEFINE_SHADER_CONSTANT_BUFFER_TYPE_INTERNAL(ConstantBufferTypeName, bInIsDynamic, false, __VA_ARGS__)

#define DEFINE_SHADER_CONSTANT_BUFFER_TYPE_ALLOW_CULL(ConstantBufferTypeName, bInIsDynamic, ...) DEFINE_SHADER_CONSTANT_BUFFER_TYPE_INTERNAL(ConstantBufferTypeName, bInIsDynamic, true, __VA_ARGS__)

#define ADD_SHADER_CONSTANT_BUFFER(ConstantBufferTypeName, VariableNameStr) \
	public: \
	FConstantBufferType##ConstantBufferTypeName VariableNameStr{this, #VariableNameStr, false};

#define ADD_SHADER_GLOBAL_CONSTANT_BUFFER(...) \
	public: \
	SHADER_CONSTANT_BUFFER_TYPE(GlobalConstantBuffer, true, true, __VA_ARGS__) GlobalConstantBuffer{this, GLOBAL_CONSTANT_BUFFER_NAME, true};

// This should match with variable declared in shader
// Recommendation : Divide frequently changed variables to another constant buffer type
#define ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_INTERNAL(Type, VariableNameStr, bInAllowCull) \
	TShaderParameterConstantBufferMemberVariable<Type> VariableNameStr{ConstructingVariable, #VariableNameStr, bInAllowCull};

#define ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Type, VariableNameStr) ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_INTERNAL(Type, VariableNameStr, false)

#define ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Type, VariableNameStr) ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_INTERNAL(Type, VariableNameStr, true)

#define ADD_SHADER_SRV_VARIABLE(VariableNameStr, InShaderParameterResourceType) \
	public: \
	FShaderParameterShaderResourceView VariableNameStr{this, #VariableNameStr, InShaderParameterResourceType, false};

#define ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(VariableNameStr, InShaderParameterResourceType) \
	public: \
	FShaderParameterShaderResourceView VariableNameStr{this, #VariableNameStr, InShaderParameterResourceType, true};

#define ADD_SHADER_UAV_VARIABLE

#define ADD_PREPROCESSOR_DEFINE(DefineStr) \
	private: \
	FShaderPreprocessorDefineAdd RD_UNIQUE_NAME(ShaderPreprocessorDefine) {*this, #DefineStr};

// This should match with declaration in common.hlsl
// @todo : this constant buffer should be allocated on default heap because it's modified once for a frame
DEFINE_SHADER_CONSTANT_BUFFER_TYPE_ALLOW_CULL(
	ViewConstantBuffer, false,
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Vector4, ViewWorldPosition)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, ViewMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, InvViewMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, ProjectionMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, InvProjectionMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, ViewProjectionMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, InvViewProjectionMatrix)
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE_ALLOW_CULL(Matrix, PrevViewProjectionMatrix)
)

// TODO) Need to support permutation?
// Example 1 : DEFINE_SHADER(MotionBlurVS, "MotionBlur.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "EARLY_OUT", "FAST_PATH=1");
// Example 2 : DEFINE_SHADER(MotionBlurPS, "MotionBlur.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None);
//
// Parameters)
// ShaderTextFileRelativePath : Shader file path relative to Asset/Shader
#define DEFINE_SHADER(ShaderName, ShaderTextFileRelativePath, ShaderEntryPoint, ShaderFrequency, ShaderCompileFlags, ...) \
	class F##ShaderName : public FD3D12Shader \
	{ \
	public: \
		F##ShaderName() = delete; \
		F##ShaderName(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath, \
			const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags) \
			: FD3D12Shader(InShaderName, InShaderTextFileRelativePath, InShaderEntryPoint, InShaderFrequency, InShaderCompileFlags) \
		{ \
			EA_ASSERT(TemplateVariable == nullptr); \
			TemplateVariable = this; \
		} \
		__VA_ARGS__ \
	public: \
		virtual void OnFinishShaderCompile() { FD3D12Shader::OnFinishShaderCompile(); ShaderParameter.Init(); } \
		virtual void ResetUsedMaterialCountForCurrentFrame() { AllocatedPerFrameMaterialCounts[GCurrentBackbufferIndex] = 0; } \
		TD3D12Material<F##ShaderName>* MakeMaterial()  \
		{ \
			EA_ASSERT_MSG(GCurrentRendererState != ERendererState::Initializing && GCurrentRendererState != ERendererState::Destroying, "\"MakeMaterialForCurrentFrame\" function can' be't be called in ERendererState::Initializing, Destroying"); \
			EA_ASSERT(TemplateVariable->IsFinishToCompile()); \
			TD3D12Material<F##ShaderName>* Material{nullptr}; \
			Material = TemplateMaterialPools.emplace_back(new TD3D12Material<F##ShaderName>(TemplateVariable)); \
			Material->Init(); \
			Material->bIsTemplateInstance = true; \
			return Material; \
		} \
		private: \
		inline static F##ShaderName* TemplateVariable = nullptr; \
		eastl::array<uint32_t, GNumBackBufferCount> AllocatedPerFrameMaterialCounts{0}; \
		eastl::vector<TD3D12Material<F##ShaderName>*> TemplateMaterialPools{}; \
		eastl::array<eastl::vector<TD3D12Material<F##ShaderName>*>, GNumBackBufferCount> PerFrameMaterialPoolList{}; \
	}; \
	static F##ShaderName ShaderName{ EA_WCHAR(#ShaderName), EA_WCHAR(ShaderTextFileRelativePath), \
		EA_WCHAR(ShaderEntryPoint), ShaderFrequency, ShaderCompileFlags };

class FD3D12ShaderManager : public EA::StdC::Singleton<FD3D12ShaderManager>, public IRendererStateCallbackInterface
{
public:

	void Init();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

	bool CompileAndAddNewShader(FD3D12Shader& Shader, const FShaderCompileArguments& InShaderCompileArguments);
	void CompileAllPendingShader();

	/// <summary>
	/// FD3D12Shader objects created through "DEFINE_SHADER" macros is added to this list
	/// </summary>
	static eastl::vector<FD3D12Shader*>& GetCompilePendingShaderList();
	static void AddCompilePendingShader(FD3D12Shader& CompilePendingShader);
	inline const eastl::vector<FD3D12Shader*>& GetCompiledShaderList() const
	{
		return CompiledShaderList;
	}
private:

	void ResetMaterialPoolOfAllShadersForCurrentFrame();

	/// <summary>
	/// FD3D12Shader objects containing compiled data is added to this list
	/// </summary>
	eastl::vector<FD3D12Shader*> CompiledShaderList;
};