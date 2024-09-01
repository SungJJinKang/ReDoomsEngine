#pragma once

#include "SceneData.hlsl"

struct MeshDrawPSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 WorldNormal : WORLD_NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBiTangent : BITANGENT;
    float2 UV0 : TEXCOORD0;
    FPrimitiveSceneData PrimitiveSceneData : PRIMITIVE_SCENE_DATA;
};