//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "../Common.hlsl"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};


float4 ColorOffset2;
bool AddOffset;
float4 ColorOffset1;

cbuffer VertexOffset
{       
    float4 Offset;
}

float4 ColorOffset3;
float4 ColorOffset4;

PSInput VSMain(float4 position : POSITION, float4 color : COLOR, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = position + Offset;
    result.color = color;
    if(AddOffset)
    {
        result.color += ColorOffset1 + ColorOffset2;
    }
    result.uv = uv;

    return result;
}

Texture2D<float4> TestTexture;

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color + ColorOffset1 + ColorOffset2 + ColorOffset3 + TestTexture.Sample(StaticPointWrapSampler, input.uv);
}