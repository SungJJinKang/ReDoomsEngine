#ifndef __SCENEDRAWCOMMON_HLSLI__
#define __SCENEDRAWCOMMON_HLSLI__

#include "SceneData.hlsl"

struct ScreenDrawPSInput
{
    float4 ScreenPosition : SV_POSITION;
    float2 UV0 : TEXCOORD0;
};

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


#endif // __SCENEDRAWCOMMON_HLSLI__