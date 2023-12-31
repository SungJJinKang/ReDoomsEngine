#include "D3D12Shader.h"
#include "ShaderCompilers/ShaderCompileHelper.h"
#include "AssetManager.h"

DECLARE_SHADER(HelloTriangleVS, "HelloTriangle.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "NO_ERROR=1");
DECLARE_SHADER(HelloTrianglePS, "HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, "NO_ERROR=1");

FD3D12Shader::FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFileRelativePath, 
	const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags, 
	const char* const InAdditionalPreprocessorDefine ...)
	:
	ShaderDeclaration(),
	ShaderCompileResult(),
	ShaderReflectionData()
{
	ShaderDeclaration.ShaderName = InShaderName;
	ShaderDeclaration.ShaderTextFileRelativePath = InShaderTextFileRelativePath;
	ShaderDeclaration.ShaderEntryPoint = InShaderEntryPoint;
	ShaderDeclaration.ShaderFrequency = InShaderFrequency;
	ShaderDeclaration.ShaderCompileFlags = InShaderCompileFlags;

	if (InAdditionalPreprocessorDefine)
	{
		auto AddToAdditionalPreprocessorDefineList = [this](const char* const Define)
		{
			ShaderDeclaration.AdditionalPreprocessorDefineList.push_back(FShaderCompileArguments::ParseDefineStr(Define));
		};
		
		va_list args;
		const char* str = InAdditionalPreprocessorDefine;
		size_t StrCount = 1;

		// Count the number of arguments
		va_start(args, InAdditionalPreprocessorDefine);
		while (str != NULL) {
			StrCount++;
			str = va_arg(args, const char*);
		}
		va_end(args);

		AddToAdditionalPreprocessorDefineList(InAdditionalPreprocessorDefine);
		// Extract and store the string arguments in the array
		va_start(args, InAdditionalPreprocessorDefine);
		for (int32_t i = 1; i < StrCount - 1; i++) 
		{
			AddToAdditionalPreprocessorDefineList(va_arg(args, const char*));
		}
		va_end(args);
	}

	FD3D12ShaderManager::AddCompilePendingShader(*this);
}

FD3D12Shader::FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFilePath,
	const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags)
	: FD3D12Shader(InShaderName, InShaderTextFilePath, InShaderEntryPoint, InShaderFrequency, InShaderCompileFlags, nullptr)
{
}

void FD3D12Shader::SetShaderCompileResult(const FShaderCompileResult& InShaderCompileResult)
{
	EA_ASSERT(!ShaderCompileResult.bIsValid); // Shouldn't overwrite
	ShaderCompileResult = InShaderCompileResult;
}

void FD3D12Shader::OnFinishShaderCompile()
{
	EA_ASSERT(!ShaderCompileResult.bIsValid);


}

void FD3D12Shader::PopulateShaderReflectionData()
{
	// ref https://rtarun9.github.io/blogs/shader_reflection/

	ID3D12ShaderReflection& D3D12ShaderReflection = *(ShaderCompileResult.DxcContainerReflection.Get());\

	{
		D3D12ShaderReflection.GetDesc(&ShaderReflectionData.ShaderDesc);
	}
	
	{
		for (uint32_t InputParameterIndex = 0; InputParameterIndex < ShaderReflectionData.ShaderDesc.InputParameters; ++InputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc{};
			D3D12ShaderReflection.GetInputParameterDesc(InputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the input element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.InputElementSemanticNameList.emplace_back(SignatureParameterDesc.SemanticName);

			D3D12_INPUT_ELEMENT_DESC InputElementDesc{};
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
	
	/*
	{
		for (uint32_t OutputParameterIndex = 0; OutputParameterIndex < ShaderReflectionData.ShaderDesc.InputParameters; ++OutputParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc{};
			D3D12ShaderReflection.GetOutputParameterDesc(OutputParameterIndex, &SignatureParameterDesc);

			// Using the semantic name provided by the signatureParameterDesc directly to the input element desc will cause the SemanticName field to have garbage values.
			// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
			ShaderReflectionData.InputElementSemanticNameList.emplace_back(SignatureParameterDesc.SemanticName);

			D3D12_INPUT_ELEMENT_DESC InputElementDesc{};
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
	*/

	{
		for (uint32_t BoundResourceIndex = 0; BoundResourceIndex < ShaderReflectionData.ShaderDesc.BoundResources; ++BoundResourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC ResourceBindingDesc{};
			VERIFYD3D12RESULT(D3D12ShaderReflection.GetResourceBindingDesc(BoundResourceIndex, &ResourceBindingDesc));

			if (ResourceBindingDesc.Type == D3D_SIT_CBUFFER)
			{
				ID3D12ShaderReflectionConstantBuffer* ShaderReflectionConstantBuffer = D3D12ShaderReflection.GetConstantBufferByIndex(BoundResourceIndex);
				D3D12_SHADER_BUFFER_DESC ConstantBufferDesc{};
				ShaderReflectionConstantBuffer->GetDesc(&ConstantBufferDesc);

				ShaderReflectionData.ConstantBufferResourceBindingDescList.emplace_back(ResourceBindingDesc);
				ShaderReflectionData.ConstantBufferDescList.emplace_back(ConstantBufferDesc);
			}
			else if (ResourceBindingDesc.Type == D3D_SIT_TEXTURE)
			{
				ShaderReflectionData.TextureResourceBindingDescList.emplace_back(ResourceBindingDesc);
			}
			else if (ResourceBindingDesc.Type == D3D_SIT_SAMPLER)
			{
				ShaderReflectionData.SamplerResourceBindingDescList.emplace_back(ResourceBindingDesc);
			}
			else
			{
				EA_ASSERT(false);
			}

			
		}
	}
}

void FD3D12ShaderManager::Init()
{
	CompileAllPendingShader();
}

bool FD3D12ShaderManager::CompileAndAddNewShader(FD3D12Shader& Shader, const FShaderCompileArguments& InShaderCompileArguments)
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

		const FShaderCompileResult ShaderCompileResult = FShaderCompileHelper::CompileShader(FinalShaderCompileArguments);
		if (ShaderCompileResult.bIsValid)
		{
			Shader.SetShaderCompileResult(ShaderCompileResult);
			bIsSuccess = true;

			RD_LOG(ELogVerbosity::Log, EA_WCHAR("Shader Compile Success : %s %s %s"), Shader.GetShaderDeclaration().ShaderName,
				Shader.GetShaderDeclaration().ShaderTextFileRelativePath, GetShaderFrequencyString(Shader.GetShaderDeclaration().ShaderFrequency));

			Shader.OnFinishShaderCompile();
		}
	}
	return bIsSuccess;
}

void FD3D12ShaderManager::CompileAllPendingShader()
{
	FShaderCompileArguments DeafulatShaderCompileArguments{};
	for (FD3D12Shader* D3D12Shader : GetCompilePendingShaderList())
	{
		CompileAndAddNewShader(*D3D12Shader, DeafulatShaderCompileArguments);
	}
	GetCompilePendingShaderList().resize(0);
}

eastl::vector<FD3D12Shader*>& FD3D12ShaderManager::GetCompilePendingShaderList()
{
	static eastl::vector<FD3D12Shader*> CompilePendingShaderList{};
	return CompilePendingShaderList;
}

void FD3D12ShaderManager::AddCompilePendingShader(FD3D12Shader& CompilePendingShader)
{
	GetCompilePendingShaderList().push_back(&CompilePendingShader);
}
