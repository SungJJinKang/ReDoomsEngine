#pragma once

#include "Common.hlsl"

#include "MeshDrawVS.hlsl"
#include "SceneTextures.hlsl"

Texture2D<float3> DiffuseTexture;
Texture2D<float3> NormalTexture;
Texture2D<float3> EmissiveTexture;

float Metalic;
float Roughness;

void MainPS(
    MeshDrawPSInput Input, 
    out float4 OutGBufferA : SV_Target0, 
    out float4 OutGBufferB : SV_Target1, 
    out float4 OutGBufferC : SV_Target2, 
    out float Depth : SV_Depth
)
{
    Depth = Input.Position.z / Input.Position.w;

    float3 NormalMapValue = NormalTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    NormalMapValue = NormalMapValue * 2.0f - 1.0f; // Transform from [0,1] to [-1,1]
    float3x3 TBN = float3x3(Input.WorldTangent, Input.WorldBiTangent, Input.WorldNormal);

    FGBufferData GBufferData;
    GBufferData.WorldNormal = normalize(mul(NormalMapValue, TBN));
    GBufferData.DiffuseColor = DiffuseTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.EmissiveColor = EmissiveTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.Metalic = Metalic;
    GBufferData.Roughness = Roughness;
    GBufferData.Depth = Depth;

    float4 GBufferA;
    float4 GBufferB;
    float4 GBufferC;
    EncodeGBufferData(GBufferData, GBufferA, GBufferB, GBufferC);
	
	OutGBufferA = GBufferA;
	OutGBufferB = GBufferB;
	OutGBufferC = GBufferC;
}