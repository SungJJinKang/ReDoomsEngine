#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"

class GBufferManager
{
public:

	eastl::shared_ptr<FD3D12Texture2DResource> SceneColorTarget;
	eastl::shared_ptr<FD3D12Texture2DResource> DepthStencilRenderTarget;

};

