#include "D3D12Enums.h"

const wchar_t* GetShaderFrequencyString(const EShaderFrequency ShaderFrequency)
{
	switch (ShaderFrequency)
	{
	case EShaderFrequency::Vertex:
		return EA_WCHAR("Vertex");
	case EShaderFrequency::Pixel:
		return EA_WCHAR("Pixel");
	case EShaderFrequency::Compute:
		return EA_WCHAR("Compute");
	default:
		EA_ASSUME(0);
	}
}