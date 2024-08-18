#pragma once

#include "Common.hlsl"

#include "ScreenDrawVS.hlsl"
#include "SceneTextures.hlsl"

void ScreenDrawPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
    FGBufferData GBufferData = FetchAndDecodeGBufferData(Input.UV0);
}