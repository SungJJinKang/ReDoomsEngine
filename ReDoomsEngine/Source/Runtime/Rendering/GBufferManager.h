#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"

class FGBufferManager
{
public:

	eastl::shared_ptr<FD3D12Texture2DResource> GBufferA;
	eastl::shared_ptr<FD3D12Texture2DResource> GBufferB;
	eastl::shared_ptr<FD3D12Texture2DResource> GBufferC;
	eastl::shared_ptr<FD3D12Texture2DResource> Depth;

};

#define ADD_GBUFFER_SHADER_SRV() \
	ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(GBufferATexture, EShaderParameterResourceType::Texture) \
	ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(GBufferBTexture, EShaderParameterResourceType::Texture) \
	ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(GBufferCTexture, EShaderParameterResourceType::Texture) \
	ADD_SHADER_SRV_VARIABLE_ALLOW_CULL(DepthTexture, EShaderParameterResourceType::Texture)