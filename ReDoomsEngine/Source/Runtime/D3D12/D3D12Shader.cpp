#include "D3D12Shader.h"

#include "ShaderCompilers/ShaderCompileHelper.h"
#include "AssetManager.h"
#include "D3D12RootSignature.h"

DEFINE_SHADER(HelloTriangleVS, "HelloTriangle.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, 
	ADD_SHADER_VARIABLE(int, Time)
	ADD_PREPROCESSOR_DEFINE(NO_ERROR=1)
	ADD_PREPROCESSOR_DEFINE(NO_ERROR1=1)
);
DEFINE_SHADER(HelloTrianglePS, "HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, ADD_PREPROCESSOR_DEFINE(NO_ERROR=1));
DEFINE_SHADER(HelloTrianglePS2, "HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, ADD_PREPROCESSOR_DEFINE(NO_ERROR = 1));

FD3D12ShaderTemplate::FD3D12ShaderTemplate(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath,
	const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags)
{
	MEM_ZERO(RootSignature);
	MEM_ZERO(ShaderDeclaration);
	MEM_ZERO(ShaderReflectionData);

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
	FD3D12RootSignatureManager::GetInstance()->GetOrCreateRootSignature(this);
}

void FD3D12ShaderTemplate::AddShaderVariable(FShaderVariableTemplate& InShaderVariable)
{
	// todo : map to shader variable in reflection data
}

void FD3D12ShaderTemplate::AddShaderPreprocessorDefine(const FShaderPreprocessorDefine& InShaderPreprocessorDefine)
{
#if RD_DEBUG
	EA_ASSERT(eastl::find(ShaderDeclaration.AdditionalPreprocessorDefineList.begin(), ShaderDeclaration.AdditionalPreprocessorDefineList.end(), InShaderPreprocessorDefine)
		== ShaderDeclaration.AdditionalPreprocessorDefineList.end());
#endif
	ShaderDeclaration.AdditionalPreprocessorDefineList.push_back(InShaderPreprocessorDefine);
}

void FD3D12ShaderTemplate::PopulateShaderReflectionData(ID3D12ShaderReflection* const InD3D12ShaderReflection)
{
	// ref https://rtarun9.github.io/blogs/shader_reflection/

	ID3D12ShaderReflection& D3D12ShaderReflection = *(InD3D12ShaderReflection);

	{
		D3D12ShaderReflection.GetDesc(&ShaderReflectionData.ShaderDesc);
	}
	
	{
		for (uint32_t InputParameterIndex = 0; InputParameterIndex < ShaderReflectionData.ShaderDesc.InputParameters; ++InputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
			MEM_ZERO(SignatureParameterDesc);
			D3D12ShaderReflection.GetInputParameterDesc(InputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the input element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.InputElementSemanticNameList.emplace_back(SignatureParameterDesc.SemanticName);

			SignatureParameterDesc.SemanticName = ShaderReflectionData.InputElementSemanticNameList.back().c_str();;
			ShaderReflectionData.InputElementSignatureParameterList.emplace_back(SignatureParameterDesc);

			D3D12_INPUT_ELEMENT_DESC InputElementDesc;
			MEM_ZERO(InputElementDesc);
			InputElementDesc.SemanticName = ShaderReflectionData.InputElementSemanticNameList.back().c_str();
			InputElementDesc.SemanticIndex = SignatureParameterDesc.SemanticIndex;
			InputElementDesc.Format = static_cast<DXGI_FORMAT>(SignatureParameterDesc.Mask);
			InputElementDesc.InputSlot = 0u;
			InputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			InputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			// There doesn't seem to be a obvious way to 
			// automate this currently, which might be a issue when instanced rendering is used 😩
			InputElementDesc.InstanceDataStepRate = 0u;
			ShaderReflectionData.InputElementDescList.emplace_back(InputElementDesc);
		}

		ShaderReflectionData.InputLayoutDesc.pInputElementDescs = ShaderReflectionData.InputElementDescList.data();
		ShaderReflectionData.InputLayoutDesc.NumElements = ShaderReflectionData.InputElementDescList.size();
	}
	
	{
		for (uint32_t OutputParameterIndex = 0; OutputParameterIndex < ShaderReflectionData.ShaderDesc.OutputParameters; ++OutputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
			MEM_ZERO(SignatureParameterDesc);
			D3D12ShaderReflection.GetOutputParameterDesc(OutputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the output element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.OutputElementSemanticNameList.emplace_back(SignatureParameterDesc.SemanticName);

			SignatureParameterDesc.SemanticName = ShaderReflectionData.OutputElementSemanticNameList.back().c_str();;
			ShaderReflectionData.OutputElementSignatureParameterList.emplace_back(SignatureParameterDesc);
		}
	}

	{
		for (uint32_t BoundResourceIndex = 0; BoundResourceIndex < ShaderReflectionData.ShaderDesc.BoundResources; ++BoundResourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc;
			MEM_ZERO(ResourceBindingDesc);

			// "ResourceBindingDesc.uFlags" mapped to "_D3D_SHADER_INPUT_FLAGS"
			VERIFYD3D12RESULT(D3D12ShaderReflection.GetResourceBindingDesc(BoundResourceIndex, &ResourceBindingDesc));

			ShaderReflectionData.ResourceBindingNameList.emplace_back(ResourceBindingDesc.Name);
			ResourceBindingDesc.Name = ShaderReflectionData.ResourceBindingNameList.back().c_str();

			switch (ResourceBindingDesc.Type)
			{
				case D3D_SIT_CBUFFER:
				{
					// Glober constant buffer's name is "Globals"

					ID3D12ShaderReflectionConstantBuffer* ShaderReflectionConstantBuffer = D3D12ShaderReflection.GetConstantBufferByIndex(BoundResourceIndex);
					D3D12_SHADER_BUFFER_DESC ConstantBufferDesc;
					MEM_ZERO(ConstantBufferDesc);
					ShaderReflectionConstantBuffer->GetDesc(&ConstantBufferDesc);

					EA_ASSERT(EA::StdC::Strcmp(ResourceBindingDesc.Name, ConstantBufferDesc.Name) == 0);

					FD3D12ConstantBufferReflectionData& ConstantBuffer = ShaderReflectionData.ConstantBufferList.emplace_back();
					ConstantBuffer.Name = ShaderReflectionData.ResourceBindingNameList.back();
					ConstantBufferDesc.Name = ConstantBuffer.Name.c_str();
					ConstantBuffer.ResourceBindingDesc = ResourceBindingDesc;
					ConstantBuffer.Desc = ConstantBufferDesc;

					for (uint32_t IndexOfVariableInConstantBuffer = 0; IndexOfVariableInConstantBuffer < ConstantBufferDesc.Variables; ++IndexOfVariableInConstantBuffer)
					{
						ID3D12ShaderReflectionVariable* const VariableInConstantBuffer = ShaderReflectionConstantBuffer->GetVariableByIndex(IndexOfVariableInConstantBuffer);

						D3D12_SHADER_VARIABLE_DESC ShaderVariableDesc;
						MEM_ZERO(ShaderVariableDesc);
						VariableInConstantBuffer->GetDesc(&ShaderVariableDesc);

						FD3D12ConstantBufferReflectionData::FD3D12VariableOfConstantBufferReflectionData& VariableOfConstantBuffer = ConstantBuffer.VariableList.emplace_back();
						VariableOfConstantBuffer.Name = ShaderVariableDesc.Name;
						ShaderVariableDesc.Name = VariableOfConstantBuffer.Name.c_str();

						VariableOfConstantBuffer.Desc = ShaderVariableDesc;
					}

					break;
				}
				case D3D_SIT_TEXTURE:
				{
					ShaderReflectionData.TextureResourceBindingDescList.emplace_back(ResourceBindingDesc);
					break;
				}
				case D3D_SIT_SAMPLER:
				{
					ShaderReflectionData.SamplerResourceBindingDescList.emplace_back(ResourceBindingDesc);
					break;
				}
				case D3D_SIT_UAV_RWTYPED:
				{
					ShaderReflectionData.TypedBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);
					break;
				}
				case D3D_SIT_BYTEADDRESS:
				{
					ShaderReflectionData.ByteAddressBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);
					break;
				}
				case D3D_SIT_UAV_RWSTRUCTURED:
				case D3D_SIT_STRUCTURED:
				{
					ShaderReflectionData.StructuredBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);
					break;
				}
				default:
					EA_ASSERT(false); // @todo support other types
			}
		}
	}
}

FShaderPreprocessorDefineAdd::FShaderPreprocessorDefineAdd(FD3D12ShaderTemplate& D3D12Shader, const char* const InDefineStr)
	: DefineStr(InDefineStr)
{
	EA_ASSERT(EA::StdC::Strstr(InDefineStr, " ") != NULL);
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
