#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"
#include "SceneTextures.hlsl"

ScreenDrawPSInput ScreenDrawVS(
    float2 Position : POSITION,
    float2 UV0 : TEXCOORD
)
{
	ScreenDrawPSInput Result;
	DrawFullScreenRect(float4(Position.xy, 0, 1), UV0, Result.ScreenPosition, Result.UV0);

    return Result;
}