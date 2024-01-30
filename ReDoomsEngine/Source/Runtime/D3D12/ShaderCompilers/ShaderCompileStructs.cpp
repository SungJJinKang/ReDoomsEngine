#include "ShaderCompileStructs.h"

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

FShaderPreprocessorDefine FShaderCompileArguments::ParseDefineStr(const char* const Str)
{
	FShaderPreprocessorDefine Result{};

	const char* const SplitChar = EA::StdC::Strchr(Str, '=');
	if (SplitChar != nullptr)
	{
		Result.Name.append_convert(Str, SplitChar - Str);
		Result.Value.append_convert(SplitChar + 1);
	}
	else
	{
		Result.Name.append_convert(Str);
	}

	return Result;
}
