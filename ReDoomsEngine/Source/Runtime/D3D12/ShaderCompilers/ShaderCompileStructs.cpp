#include "ShaderCompileStructs.h"

bool operator<(const FShaderHash& A, const FShaderHash& B)
{
	return DirectX::XMVector4Less(*reinterpret_cast<DirectX::FXMVECTOR*>(A.Value),
		*reinterpret_cast<DirectX::FXMVECTOR*>(B.Value));
}

const wchar_t* FShaderCompileArguments::ConvertShaderFrequencyToShaderProfile(const EShaderFrequency InShaderFrequency)
{
	switch (InShaderFrequency)
	{
	case EShaderFrequency::Vertex:
		return EA_WCHAR("vs_6_0");
	case EShaderFrequency::Pixel:
		return EA_WCHAR("ps_6_0");
	case EShaderFrequency::Compute:
		return EA_WCHAR("cs_6_0");
	default:
		EA_ASSUME(0);
	}
}

FShaderPreprocessorDefine FShaderCompileArguments::ParseDefineStr(const wchar_t* const Str)
{
	FShaderPreprocessorDefine Result{};

	const wchar_t* const SplitChar = EA::StdC::Strchr(Str, EA_WCHAR('='));
	if (SplitChar != nullptr)
	{
		Result.Name.append(Str, SplitChar);
		Result.Value.append(SplitChar + 1);
	}
	else
	{
		Result.Name = Str;
	}

	return Result;
}
