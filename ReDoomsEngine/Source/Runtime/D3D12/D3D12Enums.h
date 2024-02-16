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

bool IsSupportedShaderFrequency(const EShaderFrequency InShaderFrequency);

EShaderFrequency D3D12ShaderVisibilityToShaderFrequency(const D3D12_SHADER_VISIBILITY D3D12ShaderVisibility);

D3D12_SHADER_VISIBILITY ShaderFrequencyToD3D12ShaderVisibility(const EShaderFrequency ShaderFrequency);

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

ED3D12QueueType CommandAllocatorTypeTiD3D12QueueType(const ECommandAllocatorType InCommandAllocatorType);

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

enum EStaticSamplerType
{
	StaticPointWrapSampler = 0,
	StaticPointClampSampler,
	StaticLinearWrapSampler,
	StaticLinearClampSampler,
	StaticAnisotropicWrapSampler,
	StaticAnisotropicClampSampler,
	NumStaticSamplerType,
	UnknownStaticSamplerType,
};

EStaticSamplerType NameToEStaticSamplerType(const char* const InName);