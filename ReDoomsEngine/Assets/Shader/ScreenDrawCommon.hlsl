#pragma once

#include "SceneData.hlsl"

struct ScreenDrawPSInput
{
    float4 Position : SV_POSITION;
    float2 UV0 : TEXCOORD0;
};