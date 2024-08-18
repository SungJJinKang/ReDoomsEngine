#pragma once

#include "Common.hlsl"

#include "ScreenDrawVS.hlsl"
#include "SceneTextures.hlsl"

void DeferredShadingPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
    FGBufferData GBufferData = FetchAndDecodeGBufferData(Input.UV0);

	Output = float4(GBufferData.DiffuseColor, 1);
}