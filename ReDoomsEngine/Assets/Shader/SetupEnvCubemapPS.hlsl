#pragma once

#include "Common.hlsl"

#include "ScreenDrawCommon.hlsl"

Texture2D<float3> HDREnvMapTexture;
float4x4 ViewMatrixForCubemap;

void SetupEnvCubemapPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
}