#include "Common.hlsl"

#include "MeshDrawCommon.hlsl"

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float3 UV0 : TEXCOORD0;
    FPrimitiveSceneData PrimitiveSceneData : PRIMITIVE_SCENE_DATA;
};

Texture2D<float4> TriangleColorTexture;

PSInput MainVS(
    float3 Position : POSITION, 
    float3 Normal : NORMAL, 
    float3 Tangent : TANGENT, 
    float3 BiTangent : BITANGENT, 
    float3 UV0 : TEXCOORD0, // @TODO Support Multiple tex coords
    uint PrimitiveID : PRIMITIVEID
	#if INSTANCED_DRAW
    , uint InstanceID : SV_InstanceID
	#endif
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
    Result.Tangent = Tangent;
    Result.BiTangent = BiTangent;
    Result.PrimitiveSceneData = PrimitiveSceneData;

    return Result;
}

float4 MainPS(PSInput input) : SV_TARGET
{
    return TriangleColorTexture.Sample(StaticLinearWrapSampler, input.UV0);
}