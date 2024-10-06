#ifndef __CUBEMAPUTILS_HLSLI__
#define __CUBEMAPUTILS_HLSLI__

float3 GetCubemapSamplingVector(float2 InScreenPos, uint InCubemapFaceIndex, float2 InCubemapSize)
{
    float2 st = InScreenPos/InCubemapSize;
    float2 uv = 2.0 * float2(st.x, 1.0-st.y) - float2(1.0f, 1.0f);

    float3 ret;
    if(InCubemapFaceIndex == 0)      ret = float3(1.0,  uv.y, -uv.x); // right
    else if(InCubemapFaceIndex == 1) ret = float3(-1.0, uv.y,  uv.x); // left
    else if(InCubemapFaceIndex == 2) ret = float3(uv.x, 1.0, -uv.y); // up
    else if(InCubemapFaceIndex == 3) ret = float3(uv.x, -1.0, uv.y); // down
    else if(InCubemapFaceIndex == 4) ret = float3(uv.x, uv.y, 1.0); // front
    else if(InCubemapFaceIndex == 5) ret = float3(-uv.x, uv.y, -1.0); // back
    return normalize(ret);
}

float2 ConvertCubemapSamplingVectorToEquirectangularTexCoord(float3 CubemapSamplingVector)
{
	float u = atan2(CubemapSamplingVector.x, CubemapSamplingVector.z) / (2*PI) + 0.5;
	float v = CubemapSamplingVector.y * 0.5 + 0.5;
	v = -v + 1.0f;
	
	return float2(u, v);
}

#endif