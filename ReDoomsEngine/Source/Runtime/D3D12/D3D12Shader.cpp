#include "D3D12Shader.h"
#include "ShaderCompilers/ShaderCompileHelper.h"

DECLARE_SHADER(HelloTriangleVS, "Asset/Shader/HelloTriangle.hlsl", "MainVS", EShaderFrequency::Vertex, EShaderCompileFlag::None, "NO_ERROR");
DECLARE_SHADER(HelloTrianglePS, "Asset/Shader/HelloTriangle.hlsl", "MainPS", EShaderFrequency::Pixel, EShaderCompileFlag::None, "NO_ERROR");
eastl::vector<FD3D12Shader*> FD3D12ShaderManager::CompilePendingShaderList{};

FD3D12Shader::FD3D12Shader(const wchar_t* const InShaderName, const wchar_t* const InShaderTextFilePath, 
	const wchar_t* const InShaderEntryPoint, const EShaderFrequency InShaderFrequency, const uint64_t InShaderCompileFlags, 
	const char* const InAdditionalPreprocessorDefine ...)
	:
	ShaderDeclaration(),
	ShaderCompileResult()
{
	ShaderDeclaration.ShaderName = InShaderName;
	ShaderDeclaration.ShaderTextFilePath = InShaderTextFilePath;
	ShaderDeclaration.ShaderEntryPoint = InShaderEntryPoint;
	ShaderDeclaration.ShaderFrequency = InShaderFrequency;
	ShaderDeclaration.ShaderCompileFlags = InShaderCompileFlags;

	if (InAdditionalPreprocessorDefine)
	{
		va_list Args;
		va_start(Args, InAdditionalPreprocessorDefine);

		// todo : set AdditionalPreprocessorDefineList with ParseDefineStr function

		va_end(Args);
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

bool FD3D12ShaderManager::CompileAndAddNewShader(FD3D12Shader& Shader, const FShaderCompileArguments& InShaderCompileArguments)
{
	bool bIsSuccess = false;
	FShaderCompileArguments ShaderCompileArguments = InShaderCompileArguments;
	ShaderCompileArguments.ShaderDeclaration = Shader.GetShaderDeclaration();
	const FShaderCompileResult ShaderCompileResult = FShaderCompileHelper::CompileShader(InShaderCompileArguments);

	if (ShaderCompileResult.bIsValid)
	{
		Shader.SetShaderCompileResult(ShaderCompileResult);
		bIsSuccess = true;
	}
	return bIsSuccess;
}

void FD3D12ShaderManager::AddCompilePendingShader(FD3D12Shader& CompilePendingShader)
{
	CompilePendingShaderList.push_back(&CompilePendingShader);
}
