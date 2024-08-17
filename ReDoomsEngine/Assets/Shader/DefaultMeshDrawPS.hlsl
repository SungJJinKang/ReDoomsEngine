#pragma once

#include "Common.hlsl"

#include "MeshDrawCommon.hlsl"
#include "SceneTextures.hlsl"

Texture2D<float4> BaseColor;
Texture2D<float4> Emissive;
Texture2D<float> Metalic;
Texture2D<float> Roughness;
Texture2D<float> AmbientOcclusion;

void MainPS(
    PSInput Input, 
    out float4 OutGBufferA : SV_Target0, 
    out float4 OutGBufferB : SV_Target1, 
    out float4 OutGBufferC : SV_Target2, 
    out float4 OutGBufferD : SV_Target3, 
    out float Depth : SV_Depth
)
{
    Depth = Input.Position.z / Input.Position.w;

    FGBufferData GBufferData;
    GBufferData.WorldNormal = Input.WorldNormal;
    GBufferData.BaseColor = BaseColor.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.Emissive = Emissive.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.Metalic = Metalic.Sample(StaticPointClampSampler, Input.UV0).x;
    GBufferData.Roughness = Roughness.Sample(StaticPointClampSampler, Input.UV0).x;
    GBufferData.AmbientOcclusion = AmbientOcclusion.Sample(StaticPointClampSampler, Input.UV0).x;
    GBufferData.Depth = Depth;

    float4 GBufferA;
    float4 GBufferB;
    float4 GBufferC;
    float4 GBufferD;
    EncodeGBufferData(GBufferData, GBufferA, GBufferB, GBufferC, GBufferD);
	
	OutGBufferA = GBufferA;
	OutGBufferB = GBufferB;
	OutGBufferC = GBufferC;
	OutGBufferD = GBufferD;
}