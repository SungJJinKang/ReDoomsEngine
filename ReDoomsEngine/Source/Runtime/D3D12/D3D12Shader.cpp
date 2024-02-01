#include "D3D12Shader.h"

#include "ShaderCompilers/ShaderCompileHelper.h"
#include "AssetManager.h"
#include "D3D12RootSignature.h"
#include "D3D12StateCache.h"
#include "D3D12CommandContext.h"

FBoundShaderSet::FBoundShaderSet(const eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency> InShaderList)
	: ShaderList(InShaderList)
{
	CacheHash();
}

void FBoundShaderSet::CacheHash()
{
	for (uint32_t ShaderIndex = 0; ShaderIndex < EShaderFrequency::NumShaderFrequency; ++ShaderIndex)
	{
		if (ShaderList[ShaderIndex])
		{
			CachedHash = CombineHash(CachedHash, ShaderList[ShaderIndex]->GetShaderHash());
		}
	}
}

void FBoundShaderSet::Validate()
{
	bool bFound = false;

	for (FD3D12ShaderTemplate* Shader : ShaderList)
	{
		if (Shader)
		{
			bFound = true;
			break;
		}
	}

	EA_ASSERT(bFound);
}

FD3D12RootSignature* FBoundShaderSet::GetRootSignature() const
{
	return FD3D12RootSignatureManager::GetInstance()->GetOrCreateRootSignature(*this).get();
}

FD3D12ShaderTemplate::FD3D12ShaderTemplate(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath,
	const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags)
	: bFinishToCompile(false), ShaderDeclaration(), ShaderBlobData(), ShaderReflectionData(), ShaderHash(), ShaderParameterMap()
{
	ShaderDeclaration.ShaderName = InShaderName;
	ShaderDeclaration.ShaderTextFileRelativePath = InShaderTextFileRelativePath;
	ShaderDeclaration.ShaderEntryPoint = InShaderEntryPoint;
	ShaderDeclaration.ShaderFrequency = InShaderFrequency;
	ShaderDeclaration.ShaderCompileFlags = InShaderCompileFlags;

	FD3D12ShaderManager::AddCompilePendingShader(*this);
}

void FD3D12ShaderTemplate::SetShaderCompileResult(FShaderCompileResult& InShaderCompileResult)
{
	EA_ASSERT(InShaderCompileResult.bIsValid); // Shouldn't overwrite

	ShaderBlobData = eastl::move(InShaderCompileResult.ShaderBlobData);
	ShaderHash = InShaderCompileResult.ShaderHash;

}

void FD3D12ShaderTemplate::OnFinishShaderCompile()
{
	bFinishToCompile = true;
	ValidateShaderParameter();
}

void FD3D12ShaderTemplate::AddShaderParameter(FShaderParameterTemplate* InShaderParameter, const char* const InVariableName)
{
	ShaderParameterMap.emplace(InVariableName, InShaderParameter);
}

void FD3D12ShaderTemplate::AddShaderPreprocessorDefine(const FShaderPreprocessorDefine& InShaderPreprocessorDefine)
{
#if RD_DEBUG
	EA_ASSERT(eastl::find(ShaderDeclaration.AdditionalPreprocessorDefineList.begin(), ShaderDeclaration.AdditionalPreprocessorDefineList.end(), InShaderPreprocessorDefine)
		== ShaderDeclaration.AdditionalPreprocessorDefineList.end());
#endif
	ShaderDeclaration.AdditionalPreprocessorDefineList.push_back(InShaderPreprocessorDefine);
}

void FD3D12ShaderTemplate::ValidateShaderParameter()
{
	for (auto& ShaderParameterPair : ShaderParameterMap)
	{
		FShaderParameterTemplate* ShaderParameter = ShaderParameterPair.second;

		if (ShaderParameter->IsConstantBuffer())
		{
			FShaderConstantBuffer* ShaderConstantBuffer = dynamic_cast<FShaderConstantBuffer*>(ShaderParameter);
		
			auto ValidateConstantBufferReflectionData = [ShaderConstantBuffer](const FD3D12ConstantBufferReflectionData& ConstantBufferFromReflection) -> bool
			{
				bool bFoundConstantBuffer = false;
				if (ConstantBufferFromReflection.Name == ShaderConstantBuffer->GetVariableName())
				{
					bFoundConstantBuffer = true;

					for (auto& MemberVariablePair : ShaderConstantBuffer->GetMemberVariableMap())
					{
						bool bFoundMemberVariable = false;
						for (const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& MemberVariableFromReflection : ConstantBufferFromReflection.VariableList)
						{
							if (MemberVariableFromReflection.Name == MemberVariablePair.first)
							{
								bFoundMemberVariable = true;

								EA_ASSERT(MemberVariablePair.second.VariableSize == MemberVariableFromReflection.Desc.Size);
								break;
							}
						}

						EA_ASSERT(bFoundMemberVariable);
					}
				}
				return bFoundConstantBuffer;
			};

			if (ShaderConstantBuffer->IsGlobalConstantBuffer())
			{
				bool bFoundConstantBuffer = false;
				if (ValidateConstantBufferReflectionData(ShaderReflectionData.GlobalConstantBuffer))
				{
					bFoundConstantBuffer = true;
				}
				EA_ASSERT(bFoundConstantBuffer);
			}
			else
			{
				if (ShaderReflectionData.ConstantBufferList.size() > 0)
				{
					bool bFoundConstantBuffer = false;
					for (const FD3D12ConstantBufferReflectionData& ConstantBufferFromReflection : ShaderReflectionData.ConstantBufferList)
					{
						if (ValidateConstantBufferReflectionData(ConstantBufferFromReflection))
						{
							bFoundConstantBuffer = true;
							break;
						}
					}
					EA_ASSERT(bFoundConstantBuffer);
				}
			}
		}
		else
		{
			if (ShaderParameter->IsTexture())
			{

			}
			else if (ShaderParameter->IsBuffer())
			{

			}
			else if (ShaderParameter->IsSampler())
			{

			}
			else
			{
				bool bFoundMemberVariable = false;
				for (const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& MemberVariableFromReflection : ShaderReflectionData.GlobalConstantBuffer.VariableList)
				{
					if (MemberVariableFromReflection.Name == ShaderParameter->GetVariableName())
					{
						bFoundMemberVariable = true;

						EA_ASSERT(ShaderParameter->GetSize() == MemberVariableFromReflection.Desc.Size);
						break;
					}
				}

				EA_ASSERT(bFoundMemberVariable);
			}
		}
	}
}

void FD3D12ShaderTemplate::PopulateShaderReflectionData(ID3D12ShaderReflection* const InD3D12ShaderReflection)
{
	// ref https://rtarun9.github.io/blogs/shader_reflection/

	ID3D12ShaderReflection& D3D12ShaderReflection = *(InD3D12ShaderReflection);

	{
		D3D12ShaderReflection.GetDesc(&ShaderReflectionData.ShaderDesc);
	}
	
	{
		ShaderReflectionData.InputElementSemanticNameList.resize(ShaderReflectionData.ShaderDesc.InputParameters);
		ShaderReflectionData.InputElementSignatureParameterList.resize(ShaderReflectionData.ShaderDesc.InputParameters);
		ShaderReflectionData.InputElementDescList.resize(ShaderReflectionData.ShaderDesc.InputParameters);
		for (uint32_t InputParameterIndex = 0; InputParameterIndex < ShaderReflectionData.ShaderDesc.InputParameters; ++InputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
			MEM_ZERO(SignatureParameterDesc);
			D3D12ShaderReflection.GetInputParameterDesc(InputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the input element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.InputElementSemanticNameList[InputParameterIndex] = SignatureParameterDesc.SemanticName;

			SignatureParameterDesc.SemanticName = ShaderReflectionData.InputElementSemanticNameList[InputParameterIndex].c_str();;
			ShaderReflectionData.InputElementSignatureParameterList[InputParameterIndex] = SignatureParameterDesc;

			D3D12_INPUT_ELEMENT_DESC InputElementDesc;
			MEM_ZERO(InputElementDesc);
			InputElementDesc.SemanticName = ShaderReflectionData.InputElementSemanticNameList[InputParameterIndex].c_str();
			InputElementDesc.SemanticIndex = SignatureParameterDesc.SemanticIndex;
			InputElementDesc.Format = static_cast<DXGI_FORMAT>(SignatureParameterDesc.Mask);
			InputElementDesc.InputSlot = 0u;
			InputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			InputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			// There doesn't seem to be a obvious way to 
			// automate this currently, which might be a issue when instanced rendering is used 😩
			InputElementDesc.InstanceDataStepRate = 0u;
			ShaderReflectionData.InputElementDescList[InputParameterIndex] = InputElementDesc;
		}

		ShaderReflectionData.InputLayoutDesc.pInputElementDescs = ShaderReflectionData.InputElementDescList.data();
		ShaderReflectionData.InputLayoutDesc.NumElements = ShaderReflectionData.InputElementDescList.size();
	}
	
	{
		ShaderReflectionData.OutputElementSemanticNameList.resize(ShaderReflectionData.ShaderDesc.OutputParameters);
		ShaderReflectionData.OutputElementSignatureParameterList.resize(ShaderReflectionData.ShaderDesc.OutputParameters);
		for (uint32_t OutputParameterIndex = 0; OutputParameterIndex < ShaderReflectionData.ShaderDesc.OutputParameters; ++OutputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
			MEM_ZERO(SignatureParameterDesc);
			D3D12ShaderReflection.GetOutputParameterDesc(OutputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the output element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.OutputElementSemanticNameList[OutputParameterIndex] = SignatureParameterDesc.SemanticName;

			SignatureParameterDesc.SemanticName = ShaderReflectionData.OutputElementSemanticNameList[OutputParameterIndex].c_str();
			ShaderReflectionData.OutputElementSignatureParameterList[OutputParameterIndex] = SignatureParameterDesc;
		}
	}

	{
		ShaderReflectionData.ResourceBindingNameList.resize(ShaderReflectionData.ShaderDesc.BoundResources);
		for (uint32_t BoundResourceIndex = 0; BoundResourceIndex < ShaderReflectionData.ShaderDesc.BoundResources; ++BoundResourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
			MEM_ZERO(ResourceBindingDesc);

			// "ResourceBindingDesc.uFlags" mapped to "_D3D_SHADER_INPUT_FLAGS"
			VERIFYD3D12RESULT(D3D12ShaderReflection.GetResourceBindingDesc(BoundResourceIndex, &ResourceBindingDesc));

			ShaderReflectionData.ResourceBindingNameList[BoundResourceIndex] = ResourceBindingDesc.Name;
			ResourceBindingDesc.Name = ShaderReflectionData.ResourceBindingNameList[BoundResourceIndex].c_str();

			switch (ResourceBindingDesc.Type)
			{
				case D3D_SIT_CBUFFER:
				case D3D_SIT_TBUFFER:
				{
					// Glober constant buffer's name is "Globals"

					ID3D12ShaderReflectionConstantBuffer* ShaderReflectionConstantBuffer = D3D12ShaderReflection.GetConstantBufferByIndex(BoundResourceIndex);
					D3D12_SHADER_BUFFER_DESC ConstantBufferDesc;
					MEM_ZERO(ConstantBufferDesc);
					ShaderReflectionConstantBuffer->GetDesc(&ConstantBufferDesc);

					const bool bIsGlobalCBuffer = (EA::StdC::Strcmp(ResourceBindingDesc.Name, GLOBAL_CONSTANT_BUFFER_NAME) == 0);

					EA_ASSERT(EA::StdC::Strcmp(ResourceBindingDesc.Name, ConstantBufferDesc.Name) == 0);

					FD3D12ConstantBufferReflectionData& ConstantBuffer = bIsGlobalCBuffer ? ShaderReflectionData.GlobalConstantBuffer : ShaderReflectionData.ConstantBufferList.emplace_back();
					ConstantBuffer.bIsGlobalVariable = bIsGlobalCBuffer;
					ConstantBuffer.Name = ShaderReflectionData.ResourceBindingNameList[BoundResourceIndex];
					ConstantBufferDesc.Name = ConstantBuffer.Name.c_str();
					ConstantBuffer.ResourceBindingDesc = ResourceBindingDesc;
					ConstantBuffer.Desc = ConstantBufferDesc;

					ConstantBuffer.VariableList.resize(ConstantBufferDesc.Variables);
					for (uint32_t IndexOfVariableInConstantBuffer = 0; IndexOfVariableInConstantBuffer < ConstantBufferDesc.Variables; ++IndexOfVariableInConstantBuffer)
					{
						ID3D12ShaderReflectionVariable* const VariableInConstantBuffer = ShaderReflectionConstantBuffer->GetVariableByIndex(IndexOfVariableInConstantBuffer);

						D3D12_SHADER_VARIABLE_DESC ShaderParameterDesc;
						MEM_ZERO(ShaderParameterDesc);
						VariableInConstantBuffer->GetDesc(&ShaderParameterDesc);

						FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& VariableOfConstantBuffer = ConstantBuffer.VariableList[IndexOfVariableInConstantBuffer];
						VariableOfConstantBuffer.Name = ShaderParameterDesc.Name;
						ShaderParameterDesc.Name = VariableOfConstantBuffer.Name.c_str();

						VariableOfConstantBuffer.Desc = ShaderParameterDesc;
					}

					ShaderReflectionData.ConstantBufferCount = eastl::max(ShaderReflectionData.ConstantBufferCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				case D3D_SIT_TEXTURE:
				{
					ShaderReflectionData.TextureResourceBindingDescList.emplace_back(ResourceBindingDesc);

					ShaderReflectionData.ShaderResourceCount = eastl::max(ShaderReflectionData.ShaderResourceCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				case D3D_SIT_SAMPLER:
				{
					ShaderReflectionData.SamplerCount++;
					ShaderReflectionData.SamplerResourceBindingDescList.emplace_back(ResourceBindingDesc);

					ShaderReflectionData.SamplerCount = eastl::max(ShaderReflectionData.SamplerCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				case D3D_SIT_UAV_RWTYPED:
				case D3D_SIT_UAV_RWSTRUCTURED:
				case D3D_SIT_UAV_RWBYTEADDRESS:
				case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				case D3D_SIT_UAV_APPEND_STRUCTURED:
				{
					ShaderReflectionData.UnorderedAccessCount++;
					ShaderReflectionData.UAVResourceBindingDescList.emplace_back(ResourceBindingDesc);

					ShaderReflectionData.UnorderedAccessCount = eastl::max(ShaderReflectionData.UnorderedAccessCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				case D3D_SIT_BYTEADDRESS:
				{
					ShaderReflectionData.ByteAddressBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);

					ShaderReflectionData.ShaderResourceCount = eastl::max(ShaderReflectionData.ShaderResourceCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				case D3D_SIT_STRUCTURED:
				{
					ShaderReflectionData.StructuredBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);

					ShaderReflectionData.ShaderResourceCount = eastl::max(ShaderReflectionData.ShaderResourceCount, ResourceBindingDesc.BindPoint + ResourceBindingDesc.BindCount);
					break;
				}
				default:
					EA_ASSERT(false); // @todo support other types
			}
		}
	}

	ShaderReflectionData.bPopulated = true;
}

FShaderPreprocessorDefineAdd::FShaderPreprocessorDefineAdd(FD3D12ShaderTemplate& D3D12Shader, const char* const InDefineStr)
	: DefineStr(InDefineStr)
{
	EA_ASSERT_FORMATTED(EA::StdC::Strstr(InDefineStr, " ") == NULL, ("White space char is detected ( %s )", InDefineStr));
	D3D12Shader.AddShaderPreprocessorDefine(FShaderCompileArguments::ParseDefineStr(DefineStr));
}

void FD3D12ShaderManager::Init()
{
	CompileAllPendingShader();

}

bool FD3D12ShaderManager::CompileAndAddNewShader(FD3D12ShaderTemplate& Shader, const FShaderCompileArguments& InShaderCompileArguments)
{
	bool bIsSuccess = false;
	FShaderCompileArguments FinalShaderCompileArguments = InShaderCompileArguments;
	FinalShaderCompileArguments.ShaderDeclaration = Shader.GetShaderDeclaration();
	
	eastl::vector<uint8_t> OutTextData{};

	eastl::wstring AssetPath{};
	AssetPath += FAssetManager::GetShaderFolderDirectory();
	AssetPath += FinalShaderCompileArguments.ShaderDeclaration.ShaderTextFileRelativePath;
	 
	if (FAssetManager::SimpleReadEntireFile(AssetPath.data(), OutTextData))
	{
		OutTextData.emplace_back(0);
		// UTF8 to WideChar
		EA_ASSERT(EA::StdC::UTF8Validate(reinterpret_cast<const char*>(OutTextData.data()), OutTextData.size()));
		FinalShaderCompileArguments.ShaderText = eastl::u8string_view{ reinterpret_cast<const char8_t*>(OutTextData.data()), OutTextData.size() };
		FinalShaderCompileArguments.ShaderTextFilePath = AssetPath;

		FShaderCompileResult ShaderCompileResult = FShaderCompileHelper::CompileShader(FinalShaderCompileArguments);
		if (ShaderCompileResult.bIsValid)
		{
			Shader.SetShaderCompileResult(ShaderCompileResult);
			Shader.PopulateShaderReflectionData(ShaderCompileResult.DxcContainerReflection.Get());
			bIsSuccess = true;

			RD_LOG(ELogVerbosity::Log, EA_WCHAR("Shader Compile Success : %s %s %s"), Shader.GetShaderDeclaration().ShaderName,
				Shader.GetShaderDeclaration().ShaderTextFileRelativePath, GetShaderFrequencyString(Shader.GetShaderDeclaration().ShaderFrequency));

			Shader.OnFinishShaderCompile();
			
			// @todo check if name of the shader exists already
		}
	}
	return bIsSuccess;
}

void FD3D12ShaderManager::CompileAllPendingShader()
{
	FShaderCompileArguments DeafulatShaderCompileArguments{};
	for (FD3D12ShaderTemplate* D3D12Shader : GetCompilePendingShaderList())
	{
		CompileAndAddNewShader(*D3D12Shader, DeafulatShaderCompileArguments);
	}
	GetCompilePendingShaderList().resize(0);
}

eastl::vector<FD3D12ShaderTemplate*>& FD3D12ShaderManager::GetCompilePendingShaderList()
{
	static eastl::vector<FD3D12ShaderTemplate*> CompilePendingShaderList{};
	return CompilePendingShaderList;
}

void FD3D12ShaderManager::AddCompilePendingShader(FD3D12ShaderTemplate& CompilePendingShader)
{
	GetCompilePendingShaderList().push_back(&CompilePendingShader);
}

void FD3D12ShaderInstance::ApplyShaderParameter(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature)
{
	ShaderParameterContainerTemplate->ApplyShaderParameters(InCommandContext, InRootSignature);
}

void FShaderParameterContainerTemplate::Init()
{
	for (FShaderParameterTemplate* ShaderParameter : ShaderParameterList)
	{
		ShaderParameter->Init();
	}
}

void FShaderParameterContainerTemplate::AddShaderParamter(FShaderParameterTemplate* const InShaderParameter)
{
	ShaderParameterList.emplace_back(InShaderParameter);

	if (InShaderParameter->IsConstantBuffer())
	{
		ConstantBufferList.emplace_back(dynamic_cast<FShaderConstantBuffer*>(InShaderParameter));
	}
}

void FShaderParameterContainerTemplate::AllocateResources()
{
	EA_ASSERT(IsShaderInstance());

	for (FShaderParameterTemplate* ShaderParameter : ShaderParameterList)
	{
		ShaderParameter->AllocateResource();
	}
}

void FShaderParameterContainerTemplate::ApplyShaderParameters(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature)
{
	EA_ASSERT(IsShaderInstance());

	eastl::vector<FD3D12StateCache::FConstantBufferBindPointInfo> ConstantBufferBindPointInfoList{};

	for (FShaderConstantBuffer* ConstantBuffer : ConstantBufferList)
	{
		FD3D12StateCache::FConstantBufferBindPointInfo BindPoint;
		BindPoint.ConstantBufferResource = ConstantBuffer->GetConstantBufferResource();
		BindPoint.ReflectionData = ConstantBuffer->GetConstantBufferReflectionData();
	}
	InCommandContext.StateCache.ApplyConstantBuffer(InCommandContext, GetD3D12ShaderTemplate()->GetShaderFrequency(), InRootSignature, ConstantBufferBindPointInfoList);

	for (FShaderParameterTemplate* ShaderParameter : ShaderParameterList)
	{
		ShaderParameter->ApplyResource(InCommandContext, InRootSignature);
	}
}

void FShaderParameterTemplate::Init()
{
	if (IsTemplateVariable())
	{
		SetReflectionDataFromShader();
	}
}

void FShaderParameterTemplate::AllocateResource()
{
	GetD3D12Resource()->InitResource();
}

void FShaderParameterTemplate::ApplyResource(FD3D12CommandContext& const InCommandContext, const FD3D12RootSignature* const InRootSignature)
{
	// @todo : apply resource

}

void FShaderConstantBuffer::SetReflectionDataFromShader()
{
	bool bFoundReflectionData = false;

	const FD3D12ShaderReflectionData& ShaderReflection = OwnerShaderParameterContainerTemplate->GetD3D12ShaderTemplate()->GetD3D12ShaderReflection();
	if (IsGlobalConstantBuffer())
	{
		ReflectionData = &ShaderReflection.GlobalConstantBuffer;
		bFoundReflectionData = true;
	}
	else
	{
		for (const FD3D12ConstantBufferReflectionData& ConstantBufferReflectionData : ShaderReflection.ConstantBufferList)
		{
			if (ConstantBufferReflectionData.Name == GetVariableName())
			{
				ReflectionData = &ConstantBufferReflectionData;
				bFoundReflectionData = true;
				break;
			}
		}
	}

	EA_ASSERT(bFoundReflectionData);
}
