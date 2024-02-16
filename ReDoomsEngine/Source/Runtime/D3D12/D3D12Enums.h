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

enum class ECommandAllocatorType : uint32_t
{
	Graphics,

	Num
};

enum ED3D12QueueType : uint32_t
{
	Direct = 0,
	Copy,
	Async,

	NumD3D12QueueType
};

inline ED3D12QueueType CommandAllocatorTypeTiD3D12QueueType(const ECommandAllocatorType InCommandAllocatorType)
{
	switch (InCommandAllocatorType)
	{
	case ECommandAllocatorType::Graphics:
		return ED3D12QueueType::Direct;
	case ECommandAllocatorType::ResourceUploadBatcher:
		return ED3D12QueueType::Direct; // @todo : use copy queue
	default:
		EA_ASSUME(false);
	}
}

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