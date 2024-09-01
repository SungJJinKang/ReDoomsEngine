#pragma once

#include "Common.hlsl"

#include "MeshDrawCommon.hlsl"
#include "SceneTextures.hlsl"

MeshDrawPSInput MeshDrawVS(
    float3 Position : POSITION, 
    float3 Normal : NORMAL, 
    float3 Tangent : TANGENT, 
    float3 BiTangent : BITANGENT, 
    float2 UV0 : TEXCOORD, // @TODO Support Multiple tex coords
    //float3 UV1 : TEXCOORD1, // @TODO Support Multiple tex coords
    //float3 UV2 : TEXCOORD2, // @TODO Support Multiple tex coords
    //float3 UV3 : TEXCOORD3, // @TODO Support Multiple tex coords
    //float3 UV4 : TEXCOORD4, // @TODO Support Multiple tex coords
    uint PrimitiveID : PRIMITIVEID
)
{
    MeshDrawPSInput Result;

    FPrimitiveSceneData PrimitiveSceneData = GetPrimitiveIdSceneData(PrimitiveID);
    float4 NewPosition = float4(Position, 1.0f);

    NewPosition = mul(NewPosition, PrimitiveSceneData.LocalToWorld);

    Result.WorldPosition = NewPosition;

    NewPosition = mul(NewPosition, ViewProjectionMatrix);

    Result.Position = NewPosition;
    Result.UV0 = UV0;
    Result.Normal = Normal;
    float3 WorldNormal = normalize(mul(Normal, PrimitiveSceneData.LocalToWorld));
    WorldNormal.z *= -1.0f;
    Result.WorldNormal = WorldNormal;
    Result.WorldTangent = normalize(mul(Tangent, PrimitiveSceneData.LocalToWorld));
    Result.WorldBiTangent = normalize(mul(BiTangent, PrimitiveSceneData.LocalToWorld));
    Result.PrimitiveSceneData = PrimitiveSceneData;

    return Result;
}