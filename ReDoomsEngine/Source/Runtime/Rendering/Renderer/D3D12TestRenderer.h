#pragma once
#include "CommonInclude.h"
#include "Renderer/Renderer.h"

struct F3DModel;

DEFINE_SHADER_CONSTANT_BUFFER_TYPE(
	VertexOffset, true,
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(XMVECTOR, Offset)
)

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

	eastl::shared_ptr<F3DModel> Mesh;
};

