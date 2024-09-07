#ifndef __SCENEDRAWCOMMON_HLSLI__
#define __SCENEDRAWCOMMON_HLSLI__

#include "SceneData.hlsl"

struct ScreenDrawPSInput
{
    float4 ScreenPosition : SV_POSITION;
    float2 UV0 : TEXCOORD0;
};

#endif // __SCENEDRAWCOMMON_HLSLI__