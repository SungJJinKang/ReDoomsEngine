#pragma once
#include "CommonInclude.h"
#include "Renderer/Renderer.h"

struct FMeshModel;

DEFINE_SHADER_CONSTANT_BUFFER_TYPE(
	VertexOffset, true,
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Vector4, Offset)
)

struct FDrone
{
	FPrimitive Primitive;
	Vector3 OriginalPos;
};

class D3D12TestRenderer : public FRenderer
{
public:

	virtual void Init();

	void CreateRenderTargets();

	virtual void SceneSetup();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void Destroy();

private:

	FGBufferManager GBufferManager;
	bool bSetupEnvCubemap = false;

	eastl::shared_ptr<FD3D12Texture2DResource> EnvCubemap;
	eastl::shared_ptr<FD3D12Texture2DResource> HDREnvMapTexture;
};

