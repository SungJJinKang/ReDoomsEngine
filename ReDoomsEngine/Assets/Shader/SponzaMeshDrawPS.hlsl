#pragma once

#include "Common.hlsl"

#include "MeshDrawVS.hlsl"
#include "SceneTextures.hlsl"

Texture2D<float3> DiffuseTexture;
Texture2D<float3> SpecularTexture;
Texture2D<float2> NormalTexture;
Texture2D<float3> EmissiveTexture;

float Metalic;
float Roughness;

void MainPS(
    MeshDrawPSInput Input, 
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
    GBufferData.DiffuseColor = DiffuseTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.SpecularColor = SpecularTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.EmissiveColor = EmissiveTexture.Sample(StaticPointClampSampler, Input.UV0).xyz;
    GBufferData.Metalic = Metalic;
    GBufferData.Roughness = Roughness;
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