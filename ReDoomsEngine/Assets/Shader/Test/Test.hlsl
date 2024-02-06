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

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer VertexOffset
{       
    float4 Offset;
}

bool AddOffset;
float4 ColorOffset1;
float4 ColorOffset2;
float4 ColorOffset3;

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position + Offset;
    result.color = color;
    if(AddOffset)
    {
        result.color += ColorOffset1 + ColorOffset2;
    }

    return result;
}

Texture2D<float4> Texture;

float4 PSMain(PSInput input) : SV_TARGET
{
    //float4 Sample = Texture.Load(input.position);
    //return input.color + ColorOffset1 + ColorOffset2 + Sample;

    return input.color + ColorOffset1 + ColorOffset2 + ColorOffset3;
}
