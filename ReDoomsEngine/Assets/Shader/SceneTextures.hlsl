#pragma once

#include "Common.hlsl"

Texture2D<float4> GBufferA;
Texture2D<float4> GBufferB;
Texture2D<float4> GBufferC;

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
    GBufferData.BaseColor = GBufferB.Sample(StaticPointClampSampler, UV).xyz;
    GBufferData.Metalic = GBufferB.Sample(StaticPointClampSampler, UV).w;
    GBufferData.Roughness = GBufferC.Sample(StaticPointClampSampler, UV).x;
    GBufferData.AmbientOcclusion = GBufferC.Sample(StaticPointClampSampler, UV).y;
    GBufferData.Emissive = GBufferC.Sample(StaticPointClampSampler, UV).z;
    GBufferData.Depth = GBufferC.Sample(StaticPointClampSampler, UV).w;
    GBufferData.ShadingModelID = 0;
    return GBufferData;
}

void EncodeGBufferData(FGBufferData GBufferData, out float4 GBufferA, out float4 GBufferB, out float4 GBufferC)
{
    GBufferA = float4(GBufferData.WorldNormal, 0);
    GBufferB = float4(GBufferData.BaseColor, GBufferData.Metalic);
    GBufferC = float4(GBufferData.Roughness, GBufferData.AmbientOcclusion, GBufferData.Emissive, GBufferData.Depth);
}

void WriteToGBuffer(FGBufferData GBufferData, float2 UV)
{
    float4 GBufferA;
    float4 GBufferB;
    float4 GBufferC;

    EncodeGBufferData(GBufferData, GBufferA, GBufferB, GBufferC);

    GBufferA[UV] = GBufferA;
    GBufferB[UV] = GBufferB;
    GBufferC[UV] = GBufferC;
}
