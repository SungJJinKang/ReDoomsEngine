#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"

enum EShaderFrequency : uint8_t
{
	Vertex,
	Pixel,
	Compute,

	NumShaderFrequency
};

constexpr D3D12_SHADER_VISIBILITY D3D12_SHADER_VISIBILITY_START = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
constexpr D3D12_SHADER_VISIBILITY D3D12_SHADER_VISIBILITY_END = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
constexpr D3D12_SHADER_VISIBILITY D3D12_SHADER_VISIBILITY_NUM = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_AMPLIFICATION;

inline bool IsSupportedShaderFrequency(const EShaderFrequency InShaderFrequency)
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

inline EShaderFrequency D3D12ShaderVisibilityToShaderFrequency(const D3D12_SHADER_VISIBILITY D3D12ShaderVisibility)
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
		EA_ASSUME(0);
	}
}

inline D3D12_SHADER_VISIBILITY ShaderFrequencyToD3D12ShaderVisibility(const EShaderFrequency ShaderFrequency)
{
	switch (ShaderFrequency)
	{
	case EShaderFrequency::Vertex:
		return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	case EShaderFrequency::Pixel:
		return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
	default:
		EA_ASSUME(0);
	}
}

const wchar_t* GetShaderFrequencyString(const EShaderFrequency ShaderFrequency);

enum class ECommandAllocatotrType : uint32_t
{
	Graphics,
	ResourceUploadBatcher,

	Num
};
enum ERendererState : uint64_t
{
	Initializing,
	OnPreStartFrame,
	OnStartFrame,
	Draw,
	OnPreEndFrame,
	OnEndFrame,
	OnPostEndFrame,
	Destroying
};