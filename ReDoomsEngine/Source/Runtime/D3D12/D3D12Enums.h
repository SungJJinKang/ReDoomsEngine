#pragma once
#include "CommonInclude.h"

enum class EShaderFrequency : uint8_t
{
	Vertex,
	Pixel,
	Compute,
	NumShaderFrequency
};

const wchar_t* GetShaderFrequencyString(const EShaderFrequency ShaderFrequency);