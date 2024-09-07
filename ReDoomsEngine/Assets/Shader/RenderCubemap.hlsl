#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"

float4 PosScaleUVScale;
float4 InvTargetSizeAndTextureSize; 

Texture2D<float3> HDREnvMapTexture;
int CubemapFaceIndex;
float4x4 ViewMatrixForCubemap;

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
	GBufferATexture.Sample(StaticPointClampSampler, UV).xyz;
}