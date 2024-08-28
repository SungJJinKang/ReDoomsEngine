#pragma once

#include "Common.hlsl"

#include "ScreenDrawVS.hlsl"
#include "SceneTextures.hlsl"

static const float3 F0OfDielectric = 0.04;

float3 LightDirection;
float3 LightColor;

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

	// Step 2: Convert screen coordinates to NDC
    float2 NDCPos = (Input.ScreenPosition.xy / Input.ScreenPosition.xy) * 2.0f - 1.0f;

    // Step 3: Reconstruct view space position
    float4 ViewSpacePos = float4(NDCPos, GBufferData.Depth, 1.0f);

    // Step 4: Transform to world space
    float4 WorldPos = mul(ViewSpacePos, InvViewProjectionMatrix);
    WorldPos /= WorldPos.w; // Perform perspective divide

	float3 ViewDirection = normalize(ViewWorldPosition.xyz - WorldPos);
	float3 HalfVector = normalize(-LightDirection + ViewDirection);

	float3 F0 = lerp(F0OfDielectric, GBufferData.SpecularColor, GBufferData.Metalic);
	float3 Frensnel = FresnelSchlick(max(dot(GBufferData.WorldNormal, ViewDirection), 0.0), F0);
	float NDF = DistributionGGX(GBufferData.WorldNormal, HalfVector, GBufferData.Roughness);
	float Geometry = GeometrySmith(GBufferData.WorldNormal, ViewDirection, -LightDirection, (GBufferData.Roughness + 1) * (GBufferData.Roughness + 1) / 8);

	float3 SpecularReflection = NDF * Geometry * Frensnel / (4.0 * max(dot(GBufferData.WorldNormal, ViewDirection), 0.0) * max(dot(GBufferData.WorldNormal, -LightDirection), 0.0)  + 0.0001);  
	float3 DiffuseReflection = ((1.0 - Frensnel) * (1.0 - GBufferData.Metalic) * GBufferData.DiffuseColor) / PI;
	float3 Radiance = LightColor * max(dot(GBufferData.WorldNormal, -LightDirection), 0.0);

	float3 Color = (DiffuseReflection + SpecularReflection) * Radiance * dot(GBufferData.WorldNormal, -LightDirection);

	Output = float4(Color, 1);
}