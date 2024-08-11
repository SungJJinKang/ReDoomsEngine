#include "D3D12Shader.h"

#include "EASTL/algorithm.h"

#include "ShaderCompilers/ShaderCompileHelper.h"
#include "AssetManager.h"
#include "D3D12RootSignature.h"
#include "D3D12StateCache.h"
#include "D3D12CommandContext.h"
#include "ShaderCompilers/HLSLTypeHelper.h"
#include "RenderScene.h"

FBoundShaderSet::FBoundShaderSet(const eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderList)
	: ShaderInstanceList()
{
	Set(InShaderList);
}

void FBoundShaderSet::Set(const eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderList)
{
	ShaderInstanceList = InShaderList;
	for (uint32_t ShaderIndex = 0; ShaderIndex < EShaderFrequency::NumShaderFrequency; ++ShaderIndex)
	{
		if (ShaderInstanceList[ShaderIndex])
		{
			ShaderTemplateList[ShaderIndex] = ShaderInstanceList[ShaderIndex]->GetShaderTemplate();
		}
	}

	CacheHash();
	#if EA_ASSERT_ENABLED
	Validate();
	#endif
}

void FBoundShaderSet::CacheHash() const
{
	for (uint32_t ShaderIndex = 0; ShaderIndex < EShaderFrequency::NumShaderFrequency; ++ShaderIndex)
	{
		if (ShaderInstanceList[ShaderIndex])
		{
			CachedHash = CombineHash(CachedHash, ShaderInstanceList[ShaderIndex]->GetShaderTemplate()->GetShaderHash());
		}
	}

	uint128 BoundShaderSetHash;
	BoundShaderSetHash.first = CachedHash.Value[0];
	BoundShaderSetHash.second = CachedHash.Value[1];
	CachedHash64 = Hash128to64(BoundShaderSetHash);
}

void FBoundShaderSet::Validate()
{
	bool bFound = false;

	for (FD3D12ShaderInstance*& Shader : ShaderInstanceList)
	{
		if (Shader)
		{
			EA_ASSERT(Shader->GetShaderTemplate());

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

	// Important : Never call virtual function of InShaderParameter at here because this function is called from constructor of InShaderParameter's base class
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
	eastl::wstring ShaderParameterValidationLog{};

	ShaderParameterValidationLog += EA_WCHAR("-----------------------------------\n");
	ShaderParameterValidationLog += FORMATTED_WCHAR("ValidateShaderParameter(%s)", GetShaderDeclaration().ShaderName);
	ShaderParameterValidationLog += EA_WCHAR("\n");
	auto AddValidationLog = [&ShaderParameterValidationLog](const char* const InStr, const bool bEmitAssert)
	{
		ShaderParameterValidationLog += ANSI_TO_WCHAR(InStr);
		if (bEmitAssert)
		{
			RD_LOG(ELogVerbosity::Log, EA_WCHAR("%s"), ShaderParameterValidationLog.c_str());
			EA_ASSERT_MSG(!bEmitAssert, InStr);
		}
	};

	const FD3D12ShaderReflectionData& ShaderReflectionData = GetD3D12ShaderReflection();

	for (auto& ShaderParameterPair : ShaderParameterMap)
	{
		FShaderParameterTemplate* ShaderParameter = ShaderParameterPair.second;

		bool bFoundMatchingReflectionData = false;

		if (ShaderParameter->IsConstantBuffer())
		{
			const FShaderParameterConstantBuffer* const ConstantBuffer = static_cast<FShaderParameterConstantBuffer*>(ShaderParameter);

			auto ValidateConstantBuffer = [&](const FShaderParameterConstantBuffer& ConstantBuffer, const FD3D12ConstantBufferReflectionData& ReflectionData)
				{
					EA_ASSERT(EA::StdC::Strcmp(ReflectionData.Desc.Name, ConstantBuffer.GetVariableName()) == 0);

					struct FConstantBufferMemberVariableTest
					{
						const FShaderParameterConstantBuffer::FMemberVariableContainer* ShaderParameterMemberVariable = nullptr;
						const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData* ReflectionData = nullptr;
						bool bTested = false;
					};
					eastl::hash_map<eastl::string, FConstantBufferMemberVariableTest> TestedMemberVariableMap{};

					// collect possible variables
					for (const FShaderParameterConstantBuffer::FMemberVariableContainer& MemberVariablePair : ConstantBuffer.GetMemberVariableMap())
					{
						TestedMemberVariableMap.try_emplace(MemberVariablePair.VariableName).first->second.ShaderParameterMemberVariable = &MemberVariablePair;
					}
					for (const auto& MemberVariablePair : ReflectionData.VariableList)
					{
						EA_ASSERT(MemberVariablePair.first == MemberVariablePair.second.Name);
						TestedMemberVariableMap.try_emplace(MemberVariablePair.first.data()).first->second.ReflectionData = &MemberVariablePair.second;
					}

					for (auto& ConstantBufferMemberVariableTest : TestedMemberVariableMap)
					{
						EA_ASSERT(ConstantBufferMemberVariableTest.second.bTested == false);

						const FShaderParameterConstantBuffer::FMemberVariableContainer* const ShaderParameterMemberVariable = ConstantBufferMemberVariableTest.second.ShaderParameterMemberVariable;
						EA_ASSERT(ShaderParameterMemberVariable);
						const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData* const MemberVariableReflectionData = ConstantBufferMemberVariableTest.second.ReflectionData;

						if (MemberVariableReflectionData)
						{
							EA_ASSERT(*(ShaderParameterMemberVariable->VariableTypeInfo) == MemberVariableReflectionData->TypeDesc);
						}
						else
						{
							AddValidationLog(FORMATTED_CHAR("Member variable \"%s\" of ConstantBuffer(%s) is culled\n",
								ConstantBufferMemberVariableTest.second.ShaderParameterMemberVariable->VariableName, ConstantBuffer.GetVariableName()),
								!(ShaderParameterMemberVariable->ShaderParameterConstantBufferMemberVariableTemplate->IsAllowCull()));
						}
						ConstantBufferMemberVariableTest.second.bTested = true;

					
					}

					for (auto& ConstantBufferMemberVariableTest : TestedMemberVariableMap)
					{
						EA_ASSERT(ConstantBufferMemberVariableTest.second.bTested == true);
					}
				};

			if (ConstantBuffer->IsGlobalConstantBuffer())
			{
				ValidateConstantBuffer(*ConstantBuffer, ShaderReflectionData.GlobalConstantBuffer);
				bFoundMatchingReflectionData = true;
			}
			else
			{
				auto FindConstantBufferWithName = [&](const char* const Name) -> const FD3D12ConstantBufferReflectionData*
				{
					const FD3D12ConstantBufferReflectionData* Found = nullptr;

					for (const FD3D12ConstantBufferReflectionData& ReflectionData : ShaderReflectionData.ConstantBufferList)
					{
						if (STR_EQUAL(ReflectionData.Name.data(), Name))
						{
							Found = &ReflectionData;
							break;
						}
					}
					return Found;
				};

				const FD3D12ConstantBufferReflectionData* MatchingConstantBufferReflectionData = FindConstantBufferWithName(ConstantBuffer->GetVariableName());
				if (MatchingConstantBufferReflectionData)
				{
					EA_ASSERT(MatchingConstantBufferReflectionData->bIsGlobalVariable == false);
					ValidateConstantBuffer(*ConstantBuffer, *MatchingConstantBufferReflectionData);
					bFoundMatchingReflectionData = true;
				}
				else
				{
					AddValidationLog(FORMATTED_CHAR("ConstantBuffer \"%s\" is culled\n", ConstantBuffer->GetVariableName()), !(ConstantBuffer->IsAllowCull()));
				}
			}
		}
		else
		{
			bFoundMatchingReflectionData = true; //temp
			if (ShaderParameter->IsSRV())
			{
				const FShaderParameterResourceView* const SRV = static_cast<FShaderParameterResourceView*>(ShaderParameter);

				auto FindSRVithName = [&](const char* const Name) -> const D3D12_SHADER_INPUT_BIND_DESC*{
					const D3D12_SHADER_INPUT_BIND_DESC* Found = nullptr;

                    switch (SRV->GetShaderParameterResourceType())
                    {
                        case EShaderParameterResourceType::Texture:
						{
							for (const D3D12_SHADER_INPUT_BIND_DESC& ReflectionData : ShaderReflectionData.TextureResourceBindingDescList)
							{
								if (STR_EQUAL(ReflectionData.Name, Name))
								{
									Found = &ReflectionData;
									break;
								}
							}
                            break;
                        }
                        case EShaderParameterResourceType::RawBuffer:
						{
							EA_ASSERT(false);
                            break;
                        }
                        case EShaderParameterResourceType::StructuredBuffer:
						{
							for (const D3D12_SHADER_INPUT_BIND_DESC& ReflectionData : ShaderReflectionData.StructuredBufferResourceBindingDescList)
							{
								if (STR_EQUAL(ReflectionData.Name, Name))
								{
									Found = &ReflectionData;
									break;
								}
							}
                            break;
						}
						case EShaderParameterResourceType::TypedBuffer:
						{
							EA_ASSERT(false);
							break;
						}
                        default:
                        {
							EA_ASSERT(false);
                            break;
                        }
                    }

					return Found;
				};

				const D3D12_SHADER_INPUT_BIND_DESC* const MatchingSRVReflectionData = FindSRVithName(SRV->GetVariableName());
				if (MatchingSRVReflectionData)
				{
					bFoundMatchingReflectionData = true;
				}
				else
				{
					if (SRV->IsAllowCull())
					{
						AddValidationLog(FORMATTED_CHAR("SRV \"%s\" is culled\n", SRV->GetVariableName()), false);
					}
					else
					{
						AddValidationLog(FORMATTED_CHAR("SRV \"%s\" is culled(This variable isn't allowed to be culled)\n", SRV->GetVariableName()), true);
					}
				}
			}
			else if (ShaderParameter->IsUAV())
			{
				EA_ASSERT(false);

			}
			else if (ShaderParameter->IsRTV())
			{
				// @todo : implement

			}
			else
			{
				EA_ASSERT(false);
			}
		}
	}


	ShaderParameterValidationLog += EA_WCHAR("-----------------------------------");
	RD_LOG(ELogVerbosity::Log, EA_WCHAR("%s"), ShaderParameterValidationLog.c_str());
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
		ShaderReflectionData.ResourceBindingNameList.resize(ShaderReflectionData.ShaderDesc.BoundResources); // Important! : To prevent literal string name variable of resource desc from being dangling pointer
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
					ConstantBufferDesc.Name = ConstantBuffer.Name.data();
					ConstantBuffer.ResourceBindingDesc = ResourceBindingDesc;
					ConstantBuffer.Desc = ConstantBufferDesc;

					ConstantBuffer.VariableList.reserve(ConstantBufferDesc.Variables);
					for (uint32_t IndexOfVariableInConstantBuffer = 0; IndexOfVariableInConstantBuffer < ConstantBufferDesc.Variables; ++IndexOfVariableInConstantBuffer)
					{
						ID3D12ShaderReflectionVariable* const VariableInConstantBuffer = ShaderReflectionConstantBuffer->GetVariableByIndex(IndexOfVariableInConstantBuffer);
						ID3D12ShaderReflectionType* TypeReflectionData = VariableInConstantBuffer->GetType();

						D3D12_SHADER_VARIABLE_DESC ShaderParameterDesc{};
						MEM_ZERO(ShaderParameterDesc);
						VariableInConstantBuffer->GetDesc(&ShaderParameterDesc);

						D3D12_SHADER_TYPE_DESC TypeDesc{};
						MEM_ZERO(TypeDesc);
						TypeReflectionData->GetDesc(&TypeDesc);

						FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& VariableOfConstantBuffer =
							ConstantBuffer.VariableList.emplace(ShaderParameterDesc.Name, FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData{}).first->second;

						VariableOfConstantBuffer.Name = ShaderParameterDesc.Name;
						ShaderParameterDesc.Name = VariableOfConstantBuffer.Name.c_str();

						VariableOfConstantBuffer.Desc = ShaderParameterDesc;
						VariableOfConstantBuffer.TypeDesc = TypeDesc;
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

					FSamplerResourceBindingDesc SamplerResourceBindingDesc;
					SamplerResourceBindingDesc.Desc = ResourceBindingDesc;
					SamplerResourceBindingDesc.SamplerType = NameToEStaticSamplerType(ResourceBindingDesc.Name);
					EA_ASSERT(SamplerResourceBindingDesc.SamplerType < EStaticSamplerType::NumStaticSamplerType);
					ShaderReflectionData.SamplerResourceBindingDescList.emplace_back(SamplerResourceBindingDesc);

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

			EA_ASSERT_MSG(ResourceBindingDesc.BindCount == 1, "Currently multiple bindcount isn't supported yet");
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

void FD3D12ShaderManager::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
	ResetShaderInstancePoolOfAllShadersForCurrentFrame();
}

void FD3D12ShaderManager::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}

bool FD3D12ShaderManager::CompileAndAddNewShader(FD3D12ShaderTemplate& Shader, const FShaderCompileArguments& InShaderCompileArguments)
{
	bool bIsSuccess = false;
	FShaderCompileArguments FinalShaderCompileArguments = InShaderCompileArguments;
	FinalShaderCompileArguments.ShaderDeclaration = Shader.GetShaderDeclaration();
	
	eastl::vector<uint8_t> OutTextData{};

	const eastl::wstring AssetPath = FAssetManager::MakeAbsolutePathFromShaderFolder(FinalShaderCompileArguments.ShaderDeclaration.ShaderTextFileRelativePath);

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
			
			CompiledShaderList.push_back(&Shader);
			// @todo check if name of the shader exists already
		}
	}
	return bIsSuccess;
}

void FD3D12ShaderManager::CompileAllPendingShader()
{
	FShaderCompileArguments DeafulatShaderCompileArguments{};

	CompiledShaderList.reserve(GetCompilePendingShaderList().size());
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

void FD3D12ShaderManager::ResetShaderInstancePoolOfAllShadersForCurrentFrame()
{
	for (FD3D12ShaderTemplate* Shader : CompiledShaderList)
	{
		Shader->ResetUsedShaderInstanceCountForCurrentFrame();
	}
}

void FD3D12ShaderInstance::ApplyShaderParameter(FD3D12CommandContext& InCommandContext)
{
	ShaderParameterContainerTemplate->ApplyShaderParameters(InCommandContext);
}

void FD3D12ShaderInstance::ResetForReuse()
{

}

FD3D12ShaderInstance* FD3D12ShaderInstance::Duplicate() const
{
	SCOPED_CPU_TIMER(FD3D12ShaderInstance_Duplicate)

	FD3D12ShaderInstance* const DuplicatedShaderInstance = ShaderTemplate->MakeShaderInstanceForCurrentFrame();
	DuplicatedShaderInstance->ShaderParameterContainerTemplate->CopyFrom(*ShaderParameterContainerTemplate);
	return DuplicatedShaderInstance;
}

void FShaderParameterContainerTemplate::Init()
{
	for(uint32_t ShaderParameterIndex = 0 ; ShaderParameterIndex < ShaderParameterList.size() ; ++ShaderParameterIndex)
	{
		FShaderParameterTemplate* ShaderParameter = ShaderParameterList[ShaderParameterIndex];
		ShaderParameter->Init();

		if (ShaderParameter->IsSRV() && STR_EQUAL(ShaderParameter->GetVariableName(), PRIMITIVE_SCENEDATA_VARIABLE_NAME))
		{
			PrimitiveSceneDataSRVIndexInShaderParameterList = ShaderParameterIndex;
		}
	}
}

void FShaderParameterContainerTemplate::AddShaderParamter(FShaderParameterTemplate* const InShaderParameter)
{
	ShaderParameterList.emplace_back(InShaderParameter);

	// Important : Never call virtual function of InShaderParameter at here because this function is called from constructor of InShaderParameter's base class
}

void FShaderParameterContainerTemplate::ApplyShaderParameters(FD3D12CommandContext& InCommandContext)
{
	EA_ASSERT(IsShaderInstance());

	eastl::array<FD3D12ShaderResourceView*, MAX_SRVS> SRVBindPointInfoList;
	MEM_ZERO(SRVBindPointInfoList);
// 	eastl::array<FD3D12ShaderResourceView*, MAX_UAVS> UAVBindPointInfoList;
// 	MEM_ZERO(UAVBindPointInfoList);
	eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV> ConstantBufferBindPointInfoList;
	MEM_ZERO(ConstantBufferBindPointInfoList);

	for (FShaderParameterTemplate* ShaderParamter : ShaderParameterList)
	{
		if (!(ShaderParamter->IsCulled()))
		{
			if (ShaderParamter->IsConstantBuffer())
			{
				FShaderParameterConstantBuffer* ShaderParameterConstantBuffer = static_cast<FShaderParameterConstantBuffer*>(ShaderParamter);
				EA_ASSERT(ShaderParameterConstantBuffer);
				ConstantBufferBindPointInfoList[ShaderParameterConstantBuffer->GetConstantBufferReflectionData()->ResourceBindingDesc.BindPoint] = ShaderParameterConstantBuffer;
			}
			else if (ShaderParamter->IsSRV())
			{
				FShaderParameterShaderResourceView* ShaderParameterSRV = static_cast<FShaderParameterShaderResourceView*>(ShaderParamter);
				EA_ASSERT(ShaderParameterSRV);
				SRVBindPointInfoList[ShaderParameterSRV->GetReflectionData().BindPoint] = ShaderParameterSRV->GetTargetSRV();
			}
			else if (ShaderParamter->IsUAV())
			{
				EA_ASSERT_MSG(false, "Currently, it isn't supported yet");
				// 			FShaderParameterShaderResourceView* ShaderParameterSRV = static_cast<FShaderParameterShaderResourceView*>(ShaderParamter);
				// 			EA_ASSERT(ShaderParameterSRV);
				// 			UAVBindPointInfoList[ShaderParameterSRV->GetReflectionData().BindPoint] = ShaderParameterSRV;
			}
		}
		else
		{
			EA_ASSERT_FORMATTED(ShaderParamter->IsAllowCull(), ("ShaderParamter(%s) isn't allowed to be culled!", ShaderParamter->GetVariableName()));
		}
	}

	InCommandContext.StateCache.SetSRVs(GetD3D12ShaderTemplate()->GetShaderFrequency(), SRVBindPointInfoList);
	//InCommandContext.StateCache.SetUAVs(GetD3D12ShaderTemplate()->GetShaderFrequency(), UAVBindPointInfoList);
	InCommandContext.StateCache.SetConstantBuffer(GetD3D12ShaderTemplate()->GetShaderFrequency(), ConstantBufferBindPointInfoList);

}

void FShaderParameterContainerTemplate::CopyFrom(const FShaderParameterContainerTemplate& InTemplate)
{
	// Validate if this variable and passed one is referencing ShaderParameterContainerTemplate
	EA_ASSERT(GetD3D12ShaderTemplate() == InTemplate.GetD3D12ShaderTemplate());
	EA_ASSERT(ShaderParameterList.size() == InTemplate.ShaderParameterList.size());
	for (uint32_t Index = 0; Index < ShaderParameterList.size(); ++Index)
	{
		ShaderParameterList[Index]->CopyFrom(*(InTemplate.ShaderParameterList[Index]));
	}
}

FShaderParameterTemplate* FShaderParameterContainerTemplate::FindShaderParameterTemplate(const char* const InVariableName)
{
	FShaderParameterTemplate* FoundShaderParamter = nullptr;
	for (FShaderParameterTemplate* ShaderParamter : ShaderParameterList)
	{
		if (STR_EQUAL(ShaderParamter->GetVariableName(), InVariableName))
		{
			FoundShaderParamter = ShaderParamter;
			break;
		}
	}
	
	return FoundShaderParamter;
}

void FShaderParameterTemplate::CopyFrom(const FShaderParameterTemplate& InTemplate)
{
	// Validate if this variable and passed one is referencing same ShaderParameterTemplate
	EA_ASSERT(IsConstantBuffer() == InTemplate.IsConstantBuffer());
	EA_ASSERT(IsSRV() == InTemplate.IsSRV());
	EA_ASSERT(IsUAV() == InTemplate.IsUAV());
	EA_ASSERT(IsRTV() == InTemplate.IsRTV());
	EA_ASSERT(IsCulled() == InTemplate.IsCulled());
	EA_ASSERT(IsAllowCull() == InTemplate.IsAllowCull());
	EA_ASSERT(VariableName == InTemplate.VariableName);
}

void FShaderParameterTemplate::Init()
{
	SetReflectionDataFromShaderReflectionData();
}

void FShaderParameterTemplate::ApplyResource(FD3D12CommandContext& InCommandContext, const FD3D12RootSignature* const InRootSignature)
{
	// @todo : apply resource

}

FShaderParameterConstantBuffer::FShaderParameterConstantBuffer(const char* const InVariableName)
	: 
	FShaderParameterTemplate(InVariableName),
	bGlobalConstantBuffer(false),
	MemberVariableMap(), 
	ConstantBufferResource(),
	ReflectionData(nullptr), 
	ShadowData()
{

}

FShaderParameterConstantBuffer::FShaderParameterConstantBuffer(FShaderParameterContainerTemplate* InShaderParameter, const char* const InVariableName, const bool bInGlobalConstantBuffer, const bool bInIsDynamic, const bool bInAllowCull) 
	: 
	FShaderParameterTemplate(InShaderParameter, InVariableName, bInAllowCull), 
	bGlobalConstantBuffer(bInGlobalConstantBuffer),
	MemberVariableMap(), 
	ConstantBufferResource(),
	ReflectionData(nullptr),
	ShadowData(), 
	bIsDynamic(bInIsDynamic)
{

}

void FShaderParameterConstantBuffer::Init()
{
	FShaderParameterTemplate::Init();

	if (!IsCulled())
	{
		const uint32_t ConstantBufferSize = GetConstantBufferReflectionData()->Desc.Size;
		if (!IsDynamicConstantBuffer())
		{
			const FD3D12ShaderReflectionData& ShaderReflection = ShaderParameterContainerTemplate->GetD3D12ShaderTemplate()->GetD3D12ShaderReflection();
			FShaderParameterConstantBuffer* const TemplateShaderParameterConstantBuffer = GetTemplateShaderParameterConstantBuffer();
			const bool bFoundReflectionData = TemplateShaderParameterConstantBuffer->SetReflectionDataFromShaderReflectionData(ShaderReflection);

			if (bFoundReflectionData && TemplateShaderParameterConstantBuffer->ConstantBufferResource == nullptr)
			{
				TemplateShaderParameterConstantBuffer->ShadowData.resize(ConstantBufferSize);
				eastl::unique_ptr<FD3D12ConstantBufferResource> TemplateConstantBufferResource = 
					eastl::make_unique<FD3D12ConstantBufferResource>(ConstantBufferSize, false, TemplateShaderParameterConstantBuffer->ShadowData.data(), ConstantBufferSize, false, true);
				TemplateConstantBufferResource->InitResource();
				TemplateConstantBufferResource->CreateD3D12Resource();
				TemplateConstantBufferResource->SetDebugNameToResource(ANSI_TO_WCHAR(GetVariableName()));
				TemplateShaderParameterConstantBuffer->ConstantBufferResource = eastl::move(TemplateConstantBufferResource);

				for (FMemberVariableContainer& MemberVariablePair : TemplateShaderParameterConstantBuffer->MemberVariableMap)
				{
					const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& VariableOfConstantBufferReflectionData =
						TemplateShaderParameterConstantBuffer->ReflectionData->VariableList.find(MemberVariablePair.VariableName)->second; // @todo need to optimize this. #1 use vector instead of hash map. At init time, match element index with bind point from reflection data

					MemberVariablePair.ShaderParameterConstantBufferMemberVariableTemplate->SetOffset(VariableOfConstantBufferReflectionData.Desc.StartOffset);
				}
			}
		}
		else
		{
			if (!IsTemplateVariable())
			{
				ShadowData.resize(ConstantBufferSize);
				ConstantBufferResource = eastl::make_unique<FD3D12ConstantBufferResource>(ConstantBufferSize, true, ShadowData.data(), ConstantBufferSize, false);
				ConstantBufferResource->InitResource();

				for (FMemberVariableContainer& MemberVariablePair : MemberVariableMap)
				{
					const FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& VariableOfConstantBufferReflectionData =
						ReflectionData->VariableList.find(MemberVariablePair.VariableName)->second; // @todo need to optimize this. #1 use vector instead of hash map. At init time, match element index with bind point from reflection data

					MemberVariablePair.ShaderParameterConstantBufferMemberVariableTemplate->SetOffset(VariableOfConstantBufferReflectionData.Desc.StartOffset);
				}
			}
		}
	}
}

void FShaderParameterConstantBuffer::AddMemberVariable(FShaderParameterConstantBufferMemberVariableTemplate* InShaderParameterConstantBufferMemberVariable, const uint64_t InVariableSize,
	const char* const InVariableName, const std::type_info& VariableTypeInfo)
{
	EA_ASSERT(eastl::find_if(MemberVariableMap.begin(), MemberVariableMap.end(),
			[InVariableName](FMemberVariableContainer& lhs) -> bool
			{
				return lhs.VariableName == InVariableName; // Just comparing address of literal string is enough
			}) == MemberVariableMap.end()
	);

	FMemberVariableContainer MemberVariableContainer;
	MemberVariableContainer.VariableName = InVariableName;
	MemberVariableContainer.VariableSize = InVariableSize;
	MemberVariableContainer.ShaderParameterConstantBufferMemberVariableTemplate = InShaderParameterConstantBufferMemberVariable;
	MemberVariableContainer.VariableTypeInfo = &VariableTypeInfo;

	MemberVariableMap.emplace_back(MemberVariableContainer);

	// Important : Never call virtual function of InShaderParameterConstantBufferMemberVariable at here because this function is called from constructor of FShaderParameterConstantBufferMemberVariableTemplate's base class
}

void FShaderParameterConstantBuffer::CopyFrom(const FShaderParameterTemplate& InTemplate)
{
	FShaderParameterTemplate::CopyFrom(InTemplate);

	const FShaderParameterConstantBuffer& InputTemplateConstantBuffer = static_cast<const FShaderParameterConstantBuffer&>(InTemplate);

	// Validate if this variable and passed one is referencing same constant buffer
	EA_ASSERT(IsDynamicConstantBuffer() == InputTemplateConstantBuffer.IsDynamicConstantBuffer());
	EA_ASSERT(IsGlobalConstantBuffer() == InputTemplateConstantBuffer.IsGlobalConstantBuffer());
	EA_ASSERT(GetConstantBufferReflectionData() == InputTemplateConstantBuffer.GetConstantBufferReflectionData());
	EA_ASSERT(IsGlobalConstantBuffer() == InputTemplateConstantBuffer.IsGlobalConstantBuffer());
#if EA_ASSERT_ENABLED

	const eastl::vector<FMemberVariableContainer>& ThisMemberVariableMap = GetMemberVariableMap();
	const eastl::vector<FMemberVariableContainer>& InputTemplateMemberVariableMap = InputTemplateConstantBuffer.GetMemberVariableMap();
	EA_ASSERT(ThisMemberVariableMap.size() == InputTemplateMemberVariableMap.size());
	for (uint32_t Index = 0; Index < ThisMemberVariableMap.size(); ++Index)
	{
		EA_ASSERT(ThisMemberVariableMap[Index].VariableName == InputTemplateMemberVariableMap[Index].VariableName);
		EA_ASSERT(ThisMemberVariableMap[Index].VariableSize == InputTemplateMemberVariableMap[Index].VariableSize);
		EA_ASSERT(*ThisMemberVariableMap[Index].VariableTypeInfo == *InputTemplateMemberVariableMap[Index].VariableTypeInfo);
	}
#endif

	if (!IsCulled())
	{
		if (IsDynamicConstantBuffer())
		{
			ShadowData = InputTemplateConstantBuffer.ShadowData;
			MakeDirty();
		}
	}
}

uint8_t* FShaderParameterConstantBuffer::GetShadowData()
{
	if (!IsDynamicConstantBuffer())
	{
		// If non-dynamic constant buffer, return template FShaderParameterConstantBuffer's shadow data
		return  reinterpret_cast<uint8_t*>(GetTemplateShaderParameterConstantBuffer()->ShadowData.data());
	}
	else
	{
		EA_ASSERT(!IsCulled());
		return reinterpret_cast<uint8_t*>(ShadowData.data());
	}
}

FD3D12ConstantBufferResource* FShaderParameterConstantBuffer::GetConstantBufferResource()
{
	if (!IsDynamicConstantBuffer())
	{
		return GetTemplateShaderParameterConstantBuffer()->ConstantBufferResource.get();
	}
	else
	{
		EA_ASSERT(!IsCulled());
		return ConstantBufferResource.get();
	}
}

void FShaderParameterConstantBuffer::FlushShadowData()
{
	EA_ASSERT(IsDynamicConstantBuffer() ? !IsCulled() : true);
	GetConstantBufferResource()->FlushShadowData();
}

void FShaderParameterConstantBuffer::FlushShadowDataIfDirty()
{
	if (GetConstantBufferResource()->IsShadowDataDirty())
	{
		GetConstantBufferResource()->FlushShadowData();
	}
}

void FShaderParameterConstantBuffer::MakeDirty()
{
	EA_ASSERT(IsDynamicConstantBuffer() ? !IsCulled() : true);
	GetConstantBufferResource()->MakeDirty();
}

bool FShaderParameterConstantBuffer::SetReflectionDataFromShaderReflectionData(const FD3D12ShaderReflectionData& InShaderReflection)
{
	bool bFoundReflectionData = false;

	if (IsGlobalConstantBuffer())
	{
		ReflectionData = &InShaderReflection.GlobalConstantBuffer;
		bFoundReflectionData = true;
	}
	else
	{
		for (const FD3D12ConstantBufferReflectionData& ConstantBufferReflectionData : InShaderReflection.ConstantBufferList)
		{
			if (ConstantBufferReflectionData.Name == GetVariableName())
			{
				ReflectionData = &ConstantBufferReflectionData;
				bFoundReflectionData = true;
				break;
			}
		}
	}

	return bFoundReflectionData;
}

void FShaderParameterConstantBuffer::SetReflectionDataFromShaderReflectionData()
{
	// @todo : Shader Template should cache this for shader instance
	 
	bool bFoundReflectionData = false;

	const FD3D12ShaderReflectionData& ShaderReflection = ShaderParameterContainerTemplate->GetD3D12ShaderTemplate()->GetD3D12ShaderReflection();
	SetReflectionDataFromShaderReflectionData(ShaderReflection);
}

FShaderParameterShaderResourceView& FShaderParameterShaderResourceView::operator=(FD3D12ShaderResourceView* const InSRV)
{
	// @todo : check if is culled
	TargetSRV = InSRV;
	return *this;
}

void FShaderParameterShaderResourceView::CopyFrom(const FShaderParameterTemplate& InTemplate)
{
	FShaderParameterResourceView::CopyFrom(InTemplate);

	const FShaderParameterShaderResourceView& TemplateShaderParameterSRV = static_cast<const FShaderParameterShaderResourceView&>(InTemplate);

	*this = TemplateShaderParameterSRV.GetTargetSRV();
}

void FShaderParameterShaderResourceView::SetReflectionDataFromShaderReflectionData()
{
	// @todo : Shader Template should cache this for shader instance

	bool bFoundReflectionData = false;

	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> TextureResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> ByteAddressBufferResourceBindingDescList;
	eastl::vector<D3D12_SHADER_INPUT_BIND_DESC> StructuredBufferResourceBindingDescList;

	const FD3D12ShaderReflectionData& ShaderReflection = ShaderParameterContainerTemplate->GetD3D12ShaderTemplate()->GetD3D12ShaderReflection();

	if (GetShaderParameterResourceType() == EShaderParameterResourceType::Texture)
	{
		for (const D3D12_SHADER_INPUT_BIND_DESC& TextureResourceBindingDesc : ShaderReflection.TextureResourceBindingDescList)
		{
			if (EA::StdC::Strcmp(TextureResourceBindingDesc.Name, GetVariableName()) == 0)
			{
				ReflectionData = &TextureResourceBindingDesc;
				bFoundReflectionData = true;
				break;
			}
		}
	}		
	else if (GetShaderParameterResourceType() == EShaderParameterResourceType::RawBuffer)
	{
		EA_ASSERT(false);
	}
	else if (GetShaderParameterResourceType() == EShaderParameterResourceType::StructuredBuffer)
	{
		for (const D3D12_SHADER_INPUT_BIND_DESC& StructuredBufferResourceBindingDesc : ShaderReflection.StructuredBufferResourceBindingDescList)
		{
			if (EA::StdC::Strcmp(StructuredBufferResourceBindingDesc.Name, GetVariableName()) == 0)
			{
				ReflectionData = &StructuredBufferResourceBindingDesc;
				bFoundReflectionData = true;
				break;
			}
		}
	}
	else if (GetShaderParameterResourceType() == EShaderParameterResourceType::TypedBuffer)
	{
		EA_ASSERT(false);
	}
	else
	{
		EA_ASSERT(false);
	}
}

void FShaderParameterResourceView::CopyFrom(const FShaderParameterTemplate& InTemplate)
{
	FShaderParameterTemplate::CopyFrom(InTemplate);
}
