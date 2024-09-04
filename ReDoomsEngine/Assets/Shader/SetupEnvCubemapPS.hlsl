#pragma once

#include "Common.hlsl"

#include "ScreenDrawVS.hlsl"

Texture2D<float3> HDREnvMapTexture;
float4x4 ViewProjectionMatrixForCubemap;

void SetupEnvCubemapPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
}