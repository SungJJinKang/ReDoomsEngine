#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"

Texture2D<float3> HDREnvMapTexture;
int CubemapFaceIndex;
float2 CubemapSize;

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

float3 GetCubemapSamplingVector(float2 InScreenPos)
{
    float2 st = InScreenPos/CubemapSize;
    float2 uv = 2.0 * float2(st.x, 1.0-st.y) - float2(1.0f, 1.0f);

    float3 ret;
    if(CubemapFaceIndex == 0)      ret = float3(1.0,  uv.y, -uv.x); // right
    else if(CubemapFaceIndex == 1) ret = float3(-1.0, uv.y,  uv.x); // left
    else if(CubemapFaceIndex == 2) ret = float3(uv.x, 1.0, -uv.y); // up
    else if(CubemapFaceIndex == 3) ret = float3(uv.x, -1.0, uv.y); // down
    else if(CubemapFaceIndex == 4) ret = float3(uv.x, uv.y, 1.0); // front
    else if(CubemapFaceIndex == 5) ret = float3(-uv.x, uv.y, -1.0); // back
    return normalize(ret);
}

void RenderCubemapPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
	float3 CubemapSamplingVector = GetCubemapSamplingVector(Input.ScreenPosition);

	float u = atan2(CubemapSamplingVector.x, CubemapSamplingVector.z) / (2*PI) + 0.5;
	float v = CubemapSamplingVector.y * 0.5 + 0.5;
	v = -v + 1.0f;

	Output = float4(HDREnvMapTexture.Sample(StaticPointClampSampler, float2(u, v)).xyz, 1.0f);
}