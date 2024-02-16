#pragma once
#include "CommonInclude.h"
#include "Renderer.h"

class D3D12TestRenderer : public FRenderer
{
public:

	virtual void Init();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void Destroy();

private:

	float Offset = -2.0f;
	eastl::shared_ptr<FD3D12TextureResource> TestTexture;
	eastl::shared_ptr<FD3D12TextureResource> SmallTexture;
};

