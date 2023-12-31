//#include "Common.hlsl"
#include "Common.hlsl"
#include "Test/Test.hlsl"

cbuffer SceneConstantBuffer
{
    float4 offset;
    float4 padding[15];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// 테스트

#ifndef NO_ERROR
Make Compile Error!!!!!!!!!
#endif

PSInput MainVS(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position + offset;
    result.color = color;

    return result;
}

float4 MainPS(PSInput input) : SV_TARGET
{
    return input.color;
}