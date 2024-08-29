#ifndef __SCENETEXTURES_HLSLI__
#define __SCENETEXTURES_HLSLI__

#include "Common.hlsl"

Texture2D<float4> GBufferATexture;
Texture2D<float4> GBufferBTexture;
Texture2D<float4> GBufferCTexture;
Texture2D<float> DepthTexture;

struct FGBufferData
{
    float3 WorldNormal;
    float3 DiffuseColor;
    float3 EmissiveColor;
    float Metalic;
    float Roughness;
    float Depth;
    uint ShadingModelID;
};	

FGBufferData FetchAndDecodeGBufferData(float2 UV)
{
    FGBufferData GBufferData;
    GBufferData.WorldNormal = GBufferATexture.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.Metalic = GBufferATexture.Sample(StaticPointClampSampler, UV).w;

    GBufferData.DiffuseColor = GBufferBTexture.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.Roughness = GBufferBTexture.Sample(StaticPointClampSampler, UV).w;

    GBufferData.EmissiveColor = GBufferCTexture.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.ShadingModelID = asuint(GBufferCTexture.Sample(StaticPointClampSampler, UV).w);

    GBufferData.Depth = DepthTexture.Sample(StaticPointClampSampler, UV);

    return GBufferData;
}

void EncodeGBufferData(FGBufferData GBufferData, out float4 GBufferA, out float4 GBufferB, out float4 GBufferC)
{
    GBufferA = float4(GBufferData.WorldNormal, GBufferData.Metalic);
    GBufferB = float4(GBufferData.DiffuseColor, GBufferData.Roughness);
    GBufferC = float4(GBufferData.EmissiveColor, asfloat(GBufferData.ShadingModelID));
}

#endif // __SCENETEXTURES_HLSLI__