#include "D3D12Enums.h"

#include "D3D12Include.h"

bool IsSupportedShaderFrequency(const EShaderFrequency InShaderFrequency)
{
	switch (InShaderFrequency)
	{
	case EShaderFrequency::Vertex:
	case EShaderFrequency::Pixel:
		return true;
	default:
		return false;
	}
}

EShaderFrequency D3D12ShaderVisibilityToShaderFrequency(const D3D12_SHADER_VISIBILITY D3D12ShaderVisibility)
{
	switch (D3D12ShaderVisibility)
	{
	case D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX:
	{
		return EShaderFrequency::Vertex;
	}
	case D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL:
	{
		return EShaderFrequency::Pixel;
	}
	default:
		RD_ASSUME(0);
	}
}

D3D12_SHADER_VISIBILITY ShaderFrequencyToD3D12ShaderVisibility(const EShaderFrequency ShaderFrequency)
{
	switch (ShaderFrequency)
	{
	case EShaderFrequency::Vertex:
		return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	case EShaderFrequency::Pixel:
		return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
	default:
		RD_ASSUME(0);
	}
}

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
		RD_ASSUME(0);
	}
}

ED3D12QueueType CommandAllocatorTypeTiD3D12QueueType(const ECommandAllocatorType InCommandAllocatorType)
{
	switch (InCommandAllocatorType)
	{
	case ECommandAllocatorType::Graphics:
		return ED3D12QueueType::Direct;
		// 	case ECommandAllocatorType::ResourceUploadBatcher:
		// 		return ED3D12QueueType::Direct; // @todo : use copy queue
	default:
		RD_ASSUME(false);
	}
}

EStaticSamplerType NameToEStaticSamplerType(const char* const InName)
{
	if (EA::StdC::Strcmp(InName, "StaticPointWrapSampler") == 0)
	{
		return EStaticSamplerType::StaticPointWrapSampler;
	}
	else if (EA::StdC::Strcmp(InName, "StaticPointClampSampler") == 0)
	{
		return EStaticSamplerType::StaticPointClampSampler;
	}
	else if (EA::StdC::Strcmp(InName, "StaticLinearWrapSampler") == 0)
	{
		return EStaticSamplerType::StaticLinearWrapSampler;
	}
	else if (EA::StdC::Strcmp(InName, "StaticLinearClampSampler") == 0)
	{
		return EStaticSamplerType::StaticLinearClampSampler;
	}
	else if (EA::StdC::Strcmp(InName, "StaticAnisotropicWrapSampler") == 0)
	{
		return EStaticSamplerType::StaticAnisotropicWrapSampler;
	}
	else if (EA::StdC::Strcmp(InName, "StaticAnisotropicClampSampler") == 0)
	{
		return EStaticSamplerType::StaticAnisotropicClampSampler;
	}
	else
	{
		return EStaticSamplerType::DynamicSampler;
	}
}
