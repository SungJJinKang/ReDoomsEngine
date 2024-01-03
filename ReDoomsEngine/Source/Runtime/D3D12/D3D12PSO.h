#pragma once
#include "CommonInclude.h"
#include "D3D12Shader.h"

struct FD3D12BoundShaderContainer
{
	eastl::shared_ptr<FD3D12ShaderInstance> VertexShader;
	eastl::shared_ptr<FD3D12ShaderInstance> PixelShader;
	// ...
	// ...
	// ...
};

class FD3D12PSO
{

};

