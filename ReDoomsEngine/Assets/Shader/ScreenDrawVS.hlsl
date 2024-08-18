#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"
#include "SceneTextures.hlsl"

float4 PosScaleUVScale;
float4 InvTargetSizeAndTextureSize; 

void DrawFullScreenRect(
	in float4 InPosition,
	in float2 InTexCoord,
	out float4 OutPosition,
	out float2 OutTexCoord)
{
	OutPosition = InPosition;
	OutPosition.xy = (InPosition.xy * PosScaleUVScale.xy) * InvTargetSizeAndTextureSize.xy;
	OutPosition.xy *= float2(1, -1);
	OutTexCoord.xy = (InTexCoord.xy * PosScaleUVScale.zw) * InvTargetSizeAndTextureSize.zw;
}


ScreenDrawPSInput ScreenDrawVS(
    float2 Position : POSITION,
    float2 UV0 : TEXCOORD
)
{
	ScreenDrawPSInput Result;
	DrawFullScreenRect(float4(Position.xy, 0, 1), UV0, Result.Position, Result.UV0);

    return Result;
}