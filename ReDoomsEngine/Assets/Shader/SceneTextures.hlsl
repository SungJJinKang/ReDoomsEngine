#pragma once

#include "Common.hlsl"

Texture2D<float4> GBufferA;
Texture2D<float4> GBufferB;
Texture2D<float4> GBufferC;
Texture2D<float4> GBufferD;

struct FGBufferData
{
    float3 WorldNormal;
    float3 BaseColor;
    float3 Emissive;
    float Metalic;
    float Roughness;
    float AmbientOcclusion;
    float Depth;
    uint ShadingModelID;
};	

FGBufferData FetchAndDecodeGBufferData(float2 UV)
{
    FGBufferData GBufferData;
    GBufferData.WorldNormal = GBufferA.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.Metalic = GBufferA.Sample(StaticPointClampSampler, UV).w;

    GBufferData.BaseColor = GBufferB.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.Roughness = GBufferB.Sample(StaticPointClampSampler, UV).w;

    GBufferData.Emissive = GBufferC.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.AmbientOcclusion = GBufferC.Sample(StaticPointClampSampler, UV).w;

    GBufferData.ShadingModelID = asuint(GBufferD.Sample(StaticPointClampSampler, UV).x);
    GBufferData.Depth = GBufferD.Sample(StaticPointClampSampler, UV).y;

    return GBufferData;
}

void EncodeGBufferData(FGBufferData GBufferData, out float4 GBufferA, out float4 GBufferB, out float4 GBufferC, out float4 GBufferD)
{
    GBufferA = float4(GBufferData.WorldNormal, asfloat(GBufferData.Metalic));
    GBufferB = float4(GBufferData.BaseColor, GBufferData.Roughness);
    GBufferC = float4(GBufferData.Emissive, GBufferData.AmbientOcclusion);
    GBufferD = float4(asfloat(GBufferData.ShadingModelID), GBufferData.Depth, 0.0, 0.0);
}