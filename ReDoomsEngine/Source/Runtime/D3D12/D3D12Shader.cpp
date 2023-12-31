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
	ShaderCompileResult()
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

		const FShaderCompileResult ShaderCompileResult = FShaderCompileHelper::CompileShader(FinalShaderCompileArguments);
		if (ShaderCompileResult.bIsValid)
		{
			Shader.SetShaderCompileResult(ShaderCompileResult);
			bIsSuccess = true;

			RD_LOG(ELogVerbosity::Log, EA_WCHAR("Shader Compile Success : %s %s %s"), Shader.GetShaderDeclaration().ShaderName,
				Shader.GetShaderDeclaration().ShaderTextFileRelativePath, GetShaderFrequencyString(Shader.GetShaderDeclaration().ShaderFrequency));
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
