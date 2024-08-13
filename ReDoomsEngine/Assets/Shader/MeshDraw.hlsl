#pragma once

#include "Common.hlsl"

#include "MeshDrawCommon.hlsl"
#include "SceneTextures.hlsl"

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 WorldNormal : WORLD_NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float3 UV0 : TEXCOORD0;
    FPrimitiveSceneData PrimitiveSceneData : PRIMITIVE_SCENE_DATA;
};

Texture2D<float4> BaseColor;
Texture2D<float4> Emissive;
Texture2D<float> Metalic;
Texture2D<float> Roughness;
Texture2D<float> AmbientOcclusion;

PSInput MainVS(
    float3 Position : POSITION, 
    float3 Normal : NORMAL, 
    float3 Tangent : TANGENT, 
    float3 BiTangent : BITANGENT, 
    float3 UV0 : TEXCOORD0, // @TODO Support Multiple tex coords
    uint PrimitiveID : PRIMITIVEID
)
{
    PSInput Result;

    FPrimitiveSceneData PrimitiveSceneData = GetPrimitiveIdSceneData(PrimitiveID);
    float4 NewPosition = float4(Position, 1.0f);

    Normal.z *= -1.0f;
    NewPosition = mul(NewPosition, PrimitiveSceneData.LocalToWorld);

    Result.WorldPosition = NewPosition;

    NewPosition = mul(NewPosition, ViewProjectionMatrix);

    Result.Position = NewPosition;
    Result.UV0 = UV0;
    Result.Normal = Normal;
    Result.WorldNormal = mul(Normal, PrimitiveSceneData.LocalToWorld);
    Result.Tangent = Tangent;
    Result.BiTangent = BiTangent;
    Result.PrimitiveSceneData = PrimitiveSceneData;

    return Result;
}

void MainPS(
    PSInput Input, 
    out float4 OutGBufferA : SV_Target0, 
    out float4 OutGBufferB : SV_Target1, 
    out float4 OutGBufferC : SV_Target2, 
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
}