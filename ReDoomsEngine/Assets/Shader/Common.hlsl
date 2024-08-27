#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define PI 3.14159265359

// Allow only below samplers.
// if you need other sampler, add sampler at here and add proper desc to MakeStaticSamplerDescFromByName function
SamplerState StaticPointWrapSampler;
SamplerState StaticPointClampSampler;
SamplerState StaticLinearWrapSampler;
SamplerState StaticLinearClampSampler;
SamplerState StaticAnisotropicWrapSampler;
SamplerState StaticAnisotropicClampSampler;

cbuffer ViewConstantBuffer : register(b0)
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewProjectionMatrix;
	float4x4 PrevViewProjectionMatrix;
	float4x4 DirectionalLightLocalToWorld;
};

#endif // __COMMON_HLSLI__