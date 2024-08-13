#pragma once

#include "Common.hlsl"

cbuffer SceneConstantBuffer
{
    float4 offset;
    float4 padding[15];
};

Texture2D<float4> TestTexture;
float4x4 LocalToWorldMatrix;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

// 테스트

#ifndef NO_ERROR
Make Compile Error!!!!!!!!!
#endif

float4 offsetShaderVariable1;
float4 offsetShaderVariable2;
int Time;

Texture2D<float4> TriangleColorTexture;
SamplerState Sampler;

PSInput MainVS(float4 position : POSITION, float2 UV : TEXCOORD0)
{
    PSInput result;

    result.position = position + offset + LocalToWorldMatrix[0] + offsetShaderVariable1 + offsetShaderVariable2 + TestTexture.Load(int3(Time, 0, 0));
    result.UV = UV;

    return result;
}

float4 MainPS(PSInput input) : SV_TARGET
{
    return TriangleColorTexture.Sample(Sampler, input.UV);
}