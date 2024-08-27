#pragma once

#include "Common.hlsl"

#include "ScreenDrawVS.hlsl"
#include "SceneTextures.hlsl"

static const float3 F0OfDielectric = 0.04;

// references : https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

// normal distribution function
float DistributionGGX(float3 N, float3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}
//

// geometry function
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
// k : remapping of roughness
// direct lighting : (roughness + 1) * (roughness + 1) / 8
// IBL : roughness * roughness / 8
float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}
//

// fresnel
// float3 F0 = lerp(F0OfDielectric, surfaceColor.rgb, metalness);
// cosTheta = max(dot(N, H), 0.0);
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void DeferredShadingPS(
    ScreenDrawPSInput Input, 
    out float4 Output : SV_Target0
)
{
    FGBufferData GBufferData = FetchAndDecodeGBufferData(Input.UV0);

	Output = float4(GBufferData.DiffuseColor, 1);
}