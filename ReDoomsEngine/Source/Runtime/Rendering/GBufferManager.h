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
	eastl::shared_ptr<FD3D12Texture2DResource> GBufferD;
	eastl::shared_ptr<FD3D12Texture2DResource> Depth;

};

