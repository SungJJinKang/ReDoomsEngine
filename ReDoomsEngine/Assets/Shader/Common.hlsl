
// Allow only below samplers.
// if you need other sampler, add sampler at here and add proper desc to MakeStaticSamplerDescFromByName function
SamplerState StaticPointWrapSampler;
SamplerState StaticPointClampSampler;
SamplerState StaticLinearWrapSampler;
SamplerState StaticLinearClampSampler;
SamplerState StaticAnisotropicWrapSampler;
SamplerState StaticAnisotropicClampSampler;

cbuffer ViewConstantBuffer
{
	float4x4 ViewProjectionMatrix;
	float4x4 PrevViewProjectionMatrix;
}