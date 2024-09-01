#pragma once
#include "CommonInclude.h"

#include "d3d12.h"

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
	SceneSetup,
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
	StaticPointMirrorSampler,
	StaticLinearWrapSampler,
	StaticLinearClampSampler,
	StaticLinearMirrorSampler,
	StaticAnisotropicWrapSampler,
	StaticAnisotropicClampSampler,
	StaticAnisotropicMirrorSampler,
	NumStaticSamplerType,
	DynamicSampler,
	UnknownStaticSamplerType,
};

EStaticSamplerType NameToEStaticSamplerType(const char* const InName);

enum class EPipeline
{
	Graphics,
	Compute
};

enum class EShaderParameterResourceType
{
	Unknown,
	Texture,
	RawBuffer,
	StructuredBuffer,
	TypedBuffer,
};

enum ETextureFormat
{
	SceneColor = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT,
	DepthStencil = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS
};