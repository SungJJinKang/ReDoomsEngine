﻿#pragma once

#include "Common.hlsl"

#include "MeshDrawCommon.hlsl"
#include "SceneTextures.hlsl"

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