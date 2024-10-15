#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"
#include "CubemapUtils.hlsl"

TextureCube CubemapTextureCube;

ScreenDrawPSInput RenderCubemapVS(
	float2 Position : POSITION,
	float2 UV0 : TEXCOORD
)
{
	ScreenDrawPSInput Result;
	DrawFullScreenRect(float4(Position.xy, 0, 1), UV0, Result.ScreenPosition, Result.UV0);

	Result.ScreenPosition = Result.ScreenPosition.xyww;

	return Result;
}


void RenderCubemapPS(
	ScreenDrawPSInput Input,
	out float4 Output : SV_Target0
)
{
	float3 CubemapSamplingVector = GetCubemapSamplingVector(Input.ScreenPosition, CubemapFaceIndex, CubemapSize);
	Output = CubemapTextureCube.Sample(StaticPointClampSampler, CubemapSamplingVector);
}