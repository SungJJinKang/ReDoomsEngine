#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define PI 3.14159265359
#define EPSILON  0.00001

// Allow only below samplers.
// if you need other sampler, add sampler at here and add proper desc to MakeStaticSamplerDescFromByName function
SamplerState StaticPointWrapSampler;
SamplerState StaticPointClampSampler;
SamplerState StaticPointMirrorSampler;
SamplerState StaticLinearWrapSampler;
SamplerState StaticLinearClampSampler;
SamplerState StaticLinearMirrorSampler;
SamplerState StaticAnisotropicWrapSampler;
SamplerState StaticAnisotropicClampSampler;
SamplerState StaticAnisotropicMirrorSampler;

cbuffer ViewConstantBuffer : register(b0)
{
	float4 ViewWorldPosition;
	float4x4 ViewMatrix;
	float4x4 InvViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InvProjectionMatrix;
	float4x4 ViewProjectionMatrix;
	float4x4 InvViewProjectionMatrix;
	float4x4 PrevViewProjectionMatrix;
};

#endif // __COMMON_HLSLI__